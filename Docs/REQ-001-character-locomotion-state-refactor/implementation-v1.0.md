# REQ-001 角色运动状态管理重构实施文档 v1.0

状态：实施中<br>
版本：v1.0<br>
更新时间：2026-08-26<br>
关联设计：[design-v1.0.md](design-v1.0.md)

## 1. 实施目标

将角色运动输入、物理移动、Gameplay Ability、状态解析以及动画读取分别放入明确的模块中。每个步骤完成后，项目都可以编译、启动默认地图并完成对应的真实输入测试。

本次实施完成以下设计范围：

- 移动输入、Walk、Run、Sprint、Crouch、Jump、Slide、WallRun。
- Hip、ADS、Lean、Fire、Reload、Dash。
- CMC 的常规移动请求与 Movement Override 请求。
- ASC 的 Ability、Status、Block、Cooldown 结果。
- Resolver 生成 Snapshot，AnimInstance 只读取 Snapshot。
- Autonomous Proxy、Server、Simulated Proxy 的状态重建和动作事件同步。

本次实施不创建 GameFeature，不改造现有玩法模块的插拔机制。Prone、攀爬和喷气背包使用本设计预留的状态层、Custom Movement Mode 以及 Movement Override 接口，具体玩法能力放入后续需求；Dash 作为 Movement Override 的完整验证对象。

## 2. 当前代码入口

实施人员需要从当前工作区读取实际代码，不假定文件已经完成重命名。当前主要入口如下：

- `Source/Ays/Public/Player/AysPlayerController.h` 与对应 `.cpp`：绑定 Enhanced Input，当前直接持有 `ULocomotionStateComponent`。
- `Source/Ays/Public/Character/AysPlayer.h` 与对应 `.cpp`：创建角色组件，初始化 `AAysPlayerState` 上的 `UAysAbilitySystemComponent`，处理 `OnStartCrouch` 和 `OnEndCrouch`。
- `Source/Ays/Public/Player/AysPlayerState.h`：持有 `UAysAbilitySystemComponent` 与 `UAysAttributeSet`。
- `Source/Ays/Public/Component/FPSCharacterMovementComponent.h` 与对应 `.cpp`：实现 `FSavedMove_FPS`、Sprint、Slide、WallRun、Jump、Crouch 以及当前 `PerformDash`。
- `Source/Ays/Public/Component/LocomotionStateComponent.h` 与对应 `.cpp`：当前输入状态、互斥逻辑和状态委托。
- `Source/Ays/Public/Animation/FPSAnimInstance.h` 与对应 `.cpp`：当前读取状态委托和 CMC 属性，并计算动画变量。
- `Source/Ays/Private/AbilitySystem/Ability/GameplayAbility_Dash.cpp`：当前直接调用 CMC 的 `PerformDash`，同时修改旧状态组件。
- `Source/Ays/Private/AbilitySystem/Ability/GameplayAbility_WeaponBase.cpp`：当前通过旧状态组件添加和移除运动限制。
- `Source/Ays/Public/AysGameplayTags.h` 与对应 `.cpp`：当前原生 Gameplay Tag 注册位置。

如果编辑器资源引用无法从源码确认，步骤 1 的 Agent 使用 UE Python Remote Execution 查询 `/Game/FirstPerson/Lvl_FirstPerson` 的 WorldSettings、GameMode、Pawn、PlayerController、PlayerState 和 AnimBP，并将结果保存到 `Docs/REQ-001-character-locomotion-state-refactor/asset-inventory.json`。该文件只记录资源路径，不作为运行时代码依赖。

## 3. 目标模块和数据契约

### 3.1 模块归属

- `AAysPlayerController` 只接收本地输入并写入角色 Intent，或向 ASC 提交 Ability 输入。
- `ULocomotionIntentComponent` 持有当前输入意图和一次性请求序号。
- `UFPSCharacterMovementComponent` 负责位置、速度、碰撞、MovementMode、Crouch 胶囊体和网络预测。
- `UAysAbilitySystemComponent` 负责 Ability 激活、Gameplay Effect、Status、Block、Cooldown 和 ADS 结果。
- `ULocomotionResolverComponent` 读取 Intent、CMC 结果和 ASC 结果，构造完整 Snapshot。
- `FLocomotionSnapshot` 保存动画读取的完整状态，Resolver 在一次更新中整体写入。
- `UFPSAnimInstance` 只读取 Snapshot，并执行视觉插值。
- `ULocomotionStateComponent` 在迁移完成前保留兼容用途，最终从运行时路径移除。

### 3.2 `FLocomotionIntent`

在 `Source/Ays/Public/Data/LocomotionTypes.h` 中定义公共数据类型，避免 Intent、CMC、Resolver 各自声明重复字段。

`FLocomotionIntent` 至少包含：

- `FVector2D MoveInput`。
- `bool bSprintHeld`。
- `bool bCrouchHeld`。
- `bool bLeanLeftHeld`。
- `bool bLeanRightHeld`。
- `ELocomotionLeanDirection LastLeanInput`。
- `bool bADSRequested`。
- `uint32 JumpRequestId`。
- `uint32 DashRequestId`。
- `uint32 FireRequestId`。
- `uint32 ReloadRequestId`。

Intent 只表示请求。Intent 不读取碰撞，不修改速度，不写入 MovementMode，不授予 Gameplay Tag。

### 3.3 `FMovementOverrideRequest`

在同一个数据类型文件中定义 CMC 的临时运动请求：

- `uint32 RequestId`。
- `FVector Direction`。
- `float InitialSpeed`。
- `float Duration`。
- `uint8 Priority`。
- `bool bCancelOnCollision`。
- `bool bCancelOnInputRelease`。

CMC 同时只维护一个有效 Override。新请求在已有 Override 结束后提交；重复请求使用 `RequestId` 拒绝。Override 只影响 CMC 的物理执行，不写入持久 Locomotion 层。

### 3.4 `FLocomotionSnapshot`

Snapshot 至少包含：

- `PhysicalMovement`、`Stance`、`Gait`、`Aim`、`Lean`。
- `Speed2D`、移动方向、加速度大小、`bIsFalling`。
- `bWallRunIsRight`、`CrouchAlpha`。
- 当前 Action 类型、Action 序号以及有效标记。
- Snapshot 修订序号。

Snapshot 字段来源固定为：

- PhysicalMovement、Stance、Speed2D、移动方向、加速度大小、`bIsFalling`、`bWallRunIsRight`、`CrouchAlpha`：CMC 实际结果。
- Aim：ASC 的 `State.Aim.ADS`，由 Resolver 写入 Snapshot。
- Gait、Lean：Resolver 根据 Intent、CMC 结果和 ASC 限制计算。
- Action：CMC 或 ASC 的确认事件，由 Resolver 根据请求序号去重。
- Snapshot 修订序号：Resolver 递增。

### 3.5 `FLocomotionActionConfirmation`

定义一次性动作确认事件，至少包含：

- `ELocomotionActionType ActionType`。
- `uint32 RequestId`，关联 Intent 请求。
- `uint32 EventSequence`，自动开火等重复动作使用独立序号。
- `double ServerWorldTime`。
- `bool bConfirmed`。
- `ELocomotionConfirmationSource Source`，取值为 CMC 或 ASC。

CMC 发布 Jump 和 Movement Override 确认，ASC 发布 Dash、Fire、Reload 和 ADS Ability 确认。Dash 只有在相同 `DashRequestId` 的 ASC Ability 激活确认和 CMC Override 接受确认都到达后，Resolver 才生成一个 Dash Action。

### 3.6 网络传输数据

增加角色侧轻量复制结构 `FLocomotionReplicatedState`，只保存远端无法从 CMC 和 ASC 结果重建的数据：

- 服务器 Resolver 计算的 `Lean` 和 `Gait`。
- `WallRun` 侧别的可靠复制值。
- CMC 与 ASC 已确认的 Action 类型和请求序号。
- 服务器生成的 Snapshot 修订序号。

该结构挂在 `AAysPlayer` 上用于复制，字段只能由服务器侧 CMC、ASC 或 Resolver 写入。远端 Resolver 读取复制结果生成本地 Snapshot，远端 AnimInstance 不直接读取复制结构。

## 4. 实施步骤

每个步骤由一个 Agent 独立完成。Agent 开始工作前读取当前工作区和本文件，保留前序步骤的可运行状态。Agent 只修改本步骤列出的范围，并在完成后执行本步骤的构建和运行验证。

### 步骤 1：建立数据契约和角色组件

#### 修改范围

- 新增 `Source/Ays/Public/Data/LocomotionTypes.h`。
- 新增 `Source/Ays/Public/Component/LocomotionIntentComponent.h` 与对应 `.cpp`。
- 新增 `Source/Ays/Public/Component/LocomotionResolverComponent.h` 与对应 `.cpp`，本步骤只完成组件创建、引用初始化和空 Snapshot 发布接口。
- 修改 `Source/Ays/Public/Character/AysPlayer.h` 与对应 `.cpp`，在 `AAysPlayer` 上创建 Intent 和 Resolver 子对象。
- 修改 `Source/Ays/Public/Component/FPSCharacterMovementComponent.h`，增加对角色 Intent 和 Resolver 的类型引用，不改变当前移动行为。
- 修改 `Source/Ays/Public/Animation/FPSAnimInstance.h`，增加 Snapshot 读取接口的类型引用，不改变当前动画读取路径。

#### 实现内容

1. 定义状态枚举、Intent、Movement Override、Action Confirmation、Snapshot 和复制结构。
2. `ULocomotionIntentComponent` 保存一个 `FLocomotionIntent`，提供持续输入写入函数和请求序号递增函数。
3. `ULocomotionResolverComponent` 保存完整 Snapshot，提供只读访问函数和 `ResolveCurrentState` 接口。
4. `AAysPlayer` 在构造函数中创建组件，在 `BeginPlay`、`PossessedBy`、`OnRep_PlayerState`、`OnRep_Controller` 中完成可重复初始化。
5. 初始化失败使用 `checkf` 立即报告，组件重复初始化时先解除旧委托，再绑定新委托。
6. 保持 `ULocomotionStateComponent`、CMC 当前回调以及 `UFPSAnimInstance` 当前读取方式，使本步骤结束后现有行为保持可运行。

#### 验证要求

- 使用项目现有的 Unreal Editor 或 `AysEditor Win64 Development` 目标完成编译。
- 启动 `/Game/FirstPerson/Lvl_FirstPerson`，确认本地角色可以进入游戏。
- 在角色初始化后确认 Intent 和 Resolver 均有效，Snapshot 可以读取。
- 验证当前移动、下蹲、滑铲、墙走、开火、换弹和 Dash 行为没有因为组件创建发生异常。

#### 交接条件

- 新组件和数据类型已经编译通过。
- 默认地图可以启动并完成当前功能测试。
- 本步骤没有将任何输入路径切换到新组件。

### 步骤 2：迁移 Intent 到 CMC 的常规移动通道

#### 修改范围

- 修改 `Source/Ays/Public/Player/AysPlayerController.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/Component/LocomotionIntentComponent.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/Component/FPSCharacterMovementComponent.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/Character/AysPlayer.h` 与对应 `.cpp`，增加 Intent 访问函数和输入生命周期处理。
- 修改 `Source/Ays/Public/Component/LocomotionStateComponent.h` 与对应 `.cpp`，增加只执行镜像写入的过渡接口。

#### 实现内容

1. `Move` 将轴值写入 Intent。CMC 在开始模拟前读取 Intent 的移动方向，并把它转换为本次移动的加速度；`AAysPlayerController::Move` 不再同时调用 `AddMovementInput`，避免产生两条普通移动路径。
2. `SprintStart`、`SprintEnd`、`CrouchStart`、`CrouchEnd`、`LeanLeftStart`、`LeanLeftEnd`、`LeanRightStart`、`LeanRightEnd` 以及 `AimToggle` 改为更新 Intent。
3. `Jump` 递增 `JumpRequestId`。CMC 记录最近消费的请求序号，每个序号只触发一次 `AAysPlayer::Jump`。
4. CMC 从 Intent 读取 `bSprintHeld` 和 `bCrouchHeld`，维护 `GetMaxSpeed`、`Crouch`、Slide 进入条件以及 Slide 退出条件。
5. CMC 不再依赖 `AAysPlayerController` 查找 `ULocomotionStateComponent`，也不再通过旧状态委托驱动物理移动。
6. 旧状态组件继续接收镜像输入，保证 `UFPSAnimInstance` 和尚未迁移的武器 Ability 可以正常运行。镜像写入使用 `SetMirroredState(const FGameplayTag&, bool)` 这类只更新容器并广播委托的过渡接口，不调用旧互斥逻辑，也不修改 Intent。镜像写入只保留在本步骤和步骤 3 的过渡范围内。
7. Intent 的 ADS、Dash、Fire、Reload 请求序号在本步骤完成记录，Ability 触发路径留给步骤 4。

#### 具体行为

- Crouch 请求受阻时，CMC 保持实际胶囊体状态，Intent 继续保存按键状态。
- Grounded 且速度达到 Slide 阈值时，CMC 根据 Crouch 请求进入 `CMOVE_Slide`。
- `CMOVE_WallRun` 继续由 CMC 负责进入、物理过程和退出。
- Sprint 请求保持在 Intent 中，CMC 依据当前运动条件决定速度。
- Override 接口在本步骤只提供声明和空闲状态，普通移动请求继续完整执行。

#### 验证要求

- 单机 PIE 中验证 Walk、Run、Sprint 的速度和输入切换。
- 验证 Crouch 胶囊体变化、顶棚阻挡、Slide 进入和退出。
- 验证 Jump 请求只执行一次，连续按键可以生成递增请求序号。
- 验证 WallRun 进入、墙面左右侧别、墙跳和退出。
- 启用 `stat fps` 和项目现有日志，确认输入持续期间没有每帧 RPC。

#### 交接条件

- 常规移动已经由 Intent 提交给 CMC。
- 旧状态组件仍然只承担过渡镜像职责。
- 单机所有常规移动行为可以运行和重复测试。

### 步骤 3：实现 Resolver、Snapshot 和动画读取

#### 修改范围

- 修改 `Source/Ays/Public/Component/LocomotionResolverComponent.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/Component/FPSCharacterMovementComponent.h` 与对应 `.cpp`，公开只读运动结果和确认事件接口。
- 修改 `Source/Ays/Public/Animation/FPSAnimInstance.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/Character/AysPlayer.h` 与对应 `.cpp`，提供 Snapshot 只读访问。
- 修改与状态规则相关的 `Source/Ays/Public/AysGameplayTags.h` 与对应 `.cpp`。

#### 实现内容

1. Resolver 设置组件 Tick 依赖，保证 CMC 更新完成后再生成 Snapshot。每次解析在局部变量中构造完整 Snapshot，再一次性写入组件成员。
2. 按固定顺序解析：ASC 标签、CMC MovementMode 和 Crouch 结果、PhysicalMovement、Stance、Aim、Gait、Lean、Action、Snapshot 修订序号。
3. Gait 规则使用水平速度、移动输入、Stance、Aim、Status 和 Block。`Walk + ADS` 允许；Resolver 在 ADS 标签存在时将最终 Gait 发布为 Walk；CMC 的物理速度限制在步骤 4 接入 ASC 的移动限制视图；Sprint 请求继续保存在 Intent。
4. Slide 允许手动 Lean。WallRun 输出 `Lean.None`，墙面侧别只进入 `bWallRunIsRight` 和动画读取数据。
5. Resolver 只生成数据，不调用 `SetMovementMode`、`Crouch`、速度修改函数或 Ability 激活函数。
6. `UFPSAnimInstance::NativeUpdateAnimation` 只读取 Snapshot，并保留 Lean、CrouchAlpha、速度方向和其它视觉插值逻辑。
7. 移除 AnimInstance 对 `ULocomotionStateComponent` 委托的绑定，以及对 `AAysPlayerController` 的依赖。旧状态组件仍保留给步骤 4 之前的 Ability 过渡代码。
8. 注册新的状态、动作和限制标签。Resolver 在 ASC Ability 接入前，可以从旧状态组件读取 Aim、Lean 和 Sprint Block 作为过渡输入；ASC 接入后优先读取 ASC 结果，步骤 4 结束时删除这段兼容读取。旧标签不再作为新模块之间的写入接口。

#### 验证要求

- 单机 PIE 中确认 AnimInstance 在没有 PlayerController 的角色上也能初始化。
- 验证 Snapshot 的 PhysicalMovement、Stance、Gait、Aim、Lean 与实际 CMC 和 ASC 结果一致。
- 验证 Snapshot 对 Walk + ADS、Run + ADS、Slide + Lean、WallRun + Lean 四组组合的解析结果；Run + ADS 的物理速度限制在步骤 4 验证。
- 在状态切换边界连续输入，确认 Snapshot 修订序号递增，动画没有读取到半更新数据。
- 使用调试日志输出每个状态层、来源和阻止标签，确认 Resolver 可以解释每次组合结果。

#### 交接条件

- 动画已经只读取 Snapshot。
- 本地角色的常规移动和动画行为可以运行。
- Resolver 已经成为组合状态的唯一发布者。

### 步骤 4：迁移 ASC、ADS、Dash 和武器 Ability

#### 修改范围

- 修改 `Source/Ays/Public/AysGameplayTags.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/AbilitySystem/AysAbilitySystemComponent.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/AbilitySystem/Ability/GameplayAbility_Dash.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Public/AbilitySystem/Ability/GameplayAbility_WeaponBase.h` 与对应 `.cpp`。
- 修改 `Source/Ays/Private/AbilitySystem/Ability/GameplayAbility_Fire.cpp`、Reload Ability 及其公共头文件。
- 修改 `Source/Ays/Public/Component/FPSCharacterMovementComponent.h` 与对应 `.cpp`，实现 Movement Override。
- 修改 `Source/Ays/Public/Component/LocomotionResolverComponent.h` 与对应 `.cpp`，移除迁移期旧状态读取。
- 修改 `Source/Ays/Public/Player/AysPlayerController.h` 与对应 `.cpp`，接入 ADS 请求和 Ability 输入序号。

#### 实现内容

1. 添加 `State.Aim.ADS`、`Block.Movement`、`Block.Sprint`、`Block.ADS`、`Block.Lean`、`Ability.Movement.Dash` 以及对应 Action、Cooldown 标签。
2. ADS 输入写入 Intent 的 `bADSRequested`。ASC 根据该请求激活或结束 `GA_ADS`，只有 Ability 成功激活后才授予 `State.Aim.ADS`。
3. `GA_ADS` 被 Block、武器切换或角色状态终止时，ASC 移除 `State.Aim.ADS`。Resolver 读取标签并生成 Aim。
4. `UGameplayAbility_WeaponBase` 使用 Gameplay Effect 管理运动限制，不再调用 `ULocomotionStateComponent::TryAddState` 或 `RemoveState`。
5. `UGameplayAbility_Dash` 通过 CMC 的 `StartMovementOverride` 提交 `FMovementOverrideRequest`，请求中带有 `DashRequestId`。
6. CMC 在每次更新先处理有效 Override，再处理 Intent 常规请求。Override 结束后立即恢复 Intent；Override 接受和结束都发布带请求序号的确认事件。
7. Dash Action 需要同时匹配 ASC 的 Ability 激活确认和 CMC 的 Override 接受确认，Resolver 只发布一次 Dash Action。
8. Fire、Reload 的请求序号随 Ability 激活确认事件进入 Resolver。自动开火的每次射击增加独立的事件序号，输入请求序号用于关联 Ability，射击事件序号用于动画去重。Ability 的冷却、Block 和结束逻辑全部由 ASC 管理。
9. 删除 Resolver 对旧状态组件的兼容读取，确保 Aim、Lean 和 Sprint Block 都来自 Intent、CMC 或 ASC 的正式接口。
10. CMC 可以读取 ASC 提供的只读移动限制视图，例如 `Block.Sprint` 或 `MaxGait=Walk`；CMC 仍然拥有速度和位置结果的写入权。

#### 验证要求

- Walk 状态下激活 ADS，确认 `State.Aim.ADS` 和 Snapshot Aim 正确发布。
- Run 或 Sprint 状态下激活 ADS，确认 CMC 将最大步态限制为 Walk，Intent 的 Sprint 请求仍然保存。
- ADS 状态下触发 Fire 和 Reload，确认对应 Block 标签影响 Gait 和 Aim。
- 移动输入持续期间触发 Dash，确认 Override 优先执行，结束后恢复当前移动方向和 Crouch 请求。
- 重复触发 Dash，确认冷却和请求序号可以阻止重复执行。
- 触发 Fire、Reload、Dash，确认每个 Action 只产生一次确认事件。

#### 交接条件

- ADS、Dash、Fire、Reload 不再写入旧状态组件。
- CMC 的两条请求通道可以独立测试：Intent 常规请求、ASC Movement Override 请求。
- ASC 是 Ability、Status、Block、Cooldown 和 ADS 结果的唯一事实来源。

### 步骤 5：网络同步、远端动画和旧组件移除

#### 修改范围

- 修改 `Source/Ays/Public/Character/AysPlayer.h` 与对应 `.cpp`，增加 `FLocomotionReplicatedState` 和复制通知。
- 修改 `Source/Ays/Public/Component/FPSCharacterMovementComponent.h` 与对应 `.cpp`，完成 SavedMove、自定义运动结果和确认事件同步。
- 修改 `Source/Ays/Public/Component/LocomotionResolverComponent.h` 与对应 `.cpp`，支持服务器结果和远端复制结果。
- 修改 `Source/Ays/Public/Animation/FPSAnimInstance.h` 与对应 `.cpp`，删除所有 Controller 状态依赖。
- 修改 `Source/Ays/Public/Player/AysPlayerController.h` 与对应 `.cpp`，删除旧状态组件属性和输入镜像写入。
- 修改所有 Ability 对 `ULocomotionStateComponent` 的引用。
- 删除 `Source/Ays/Public/Component/LocomotionStateComponent.h` 与对应 `.cpp`。

#### 实现内容

1. 扩展 `FSavedMove_FPS`，保存 Sprint、Crouch、Lean、Jump 请求序号以及自定义 Movement Mode 所需的输入结果。字段保持可压缩，避免逐帧发送完整 Intent。
2. CMC 服务器重新执行碰撞、速度、Slide、WallRun、Jump 和 Movement Override。客户端预测结果经过 Unreal Movement Correction 后与服务器结果保持一致。
3. ASC 使用 GAS 的预测和复制处理 Ability 激活、Gameplay Effect、Cooldown、Block 以及确认事件。
4. 服务器 Resolver 将 Lean、Gait、WallRun 侧别和无法重建的 Action 写入 `FLocomotionReplicatedState`。
5. 远端 Resolver 读取复制的 CMC 结果、ASC 标签和轻量复制字段，生成与本地相同格式的 Snapshot。
6. `UFPSAnimInstance` 在 Simulated Proxy 上不查找 `AAysPlayerController`，不依赖本地输入，不绑定旧状态委托。
7. Action 确认事件使用请求序号和 Snapshot 修订序号去重。远端动画记录最后消费的序号，重复网络包不能重复播放事件。
8. 删除 `AAysPlayerController`、CMC、AnimInstance、Dash Ability、Weapon Ability 对 `ULocomotionStateComponent` 的全部引用，再删除旧组件文件。
9. 使用 `rg` 检查旧类名只出现在迁移记录或历史文档中；运行时代码不得继续包含旧状态组件引用。

#### 网络验证要求

- 使用 Listen Server 加一名拥有者客户端和一名远端客户端，验证 Walk、Run、Sprint、Crouch、Slide、WallRun、Jump。
- 验证拥有者客户端预测 Dash，服务器确认后远端客户端只播放一次 Dash Action。
- 使用 Unreal 网络调试命令设置延迟和丢包，确认客户端可以继续预测，服务器校正后 Snapshot 能恢复一致。
- 验证远端角色没有本地 `AAysPlayerController` 时，Resolver、Snapshot 和 AnimInstance 正常初始化。
- 验证服务器拒绝非法 Ability、Cooldown、Block 和 Movement Override 请求。
- 验证重新占有角色、`OnRep_PlayerState` 和 `OnRep_Controller` 只绑定一次委托。

#### 交接条件

- 三种网络角色都能生成 Snapshot 和动画结果。
- 旧状态组件已经从运行时代码删除。
- 默认地图和双客户端网络测试可以重复运行。

## 5. 每个步骤的交付格式

Agent 完成对应步骤后，交付以下内容：

- 修改文件清单，以及每个文件的职责变化。
- 构建命令和构建结果。
- 实际运行的地图、网络模式、输入组合和观察结果。
- 失败场景、服务器日志和客户端日志中的关键错误。
- 下一步骤需要关注的接口或数据字段。

交接内容只记录当前工作区的最终状态，避免保留无效方案和临时回退说明。

## 6. 统一验收场景

所有步骤完成后，使用真实 Unreal PIE 或独立服务器执行以下场景：

1. Idle、Walk、Run、Sprint 之间连续切换，确认 Gait 每次只有一个值。
2. Walk + ADS、Run + ADS、Sprint + ADS，确认 ADS 约束和最大速度结果一致。
3. Crouch 进入、解除、顶棚阻挡、Slide 进入和 Slide 退出。
4. WallRun 左侧、右侧、墙跳、退出，以及 WallRun 期间 Lean 被禁止。
5. Lean 左右同时按下和快速交替输入，确认使用最近一次有效方向。
6. Dash 期间保持移动、Crouch、ADS 请求，确认 Override、Block 和恢复规则一致。
7. Fire、Reload、Dash 连续触发，确认冷却、Block、Ability 确认和 Action 去重正常。
8. Listen Server、拥有者客户端、Simulated Proxy 同时观察 Snapshot，确认每个角色都能在没有本地 Controller 的情况下完成动画读取。

## 7. 未包含的工作

- GameFeature 的注册、激活、停用和资源插拔。
- Prone、攀爬、喷气背包的具体 Ability、输入、物理参数、特效和动画资产。
- 新的动画资产、运动参数调优、体力、载具、游泳、梯子和 Zipline。
- 与本次状态数据流无关的武器命中、伤害计算和 UI 重构。

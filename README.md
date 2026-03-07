# AYShooter

引擎版本：UE5.7

游戏类型：第一人称多人FPS

## Demo演示

<video src="Markdown/Demo.mp4"></video>

## 简介

### 3C

1. FPP/TPP双骨骼，FPP适配客户端第一人称视角，TPP作为主骨骼（待实现）搭载RootMotion等功能；

2. 支持左右探头、冲刺、滑铲、喷气背包等功能；

3. 参考GASP实现了简陋的攀爬。

### 武器系统

1. 用GAS插件实现武器装备、射击、换弹等功能，采用结合GAS的网络同步方案；
2. 采用尽量Data-Driven的设计，目前只有一把步枪的资源，后续应该可以比较容易地扩展。

### UI

1. 采用MVC架构，委托驱动Widget更新。

## TODO

1. 继续扩展CMC的自定义移动Mode实现TTF里的墙走功能
2. 完善网络同步
3. 使用MotionMatch作为TPP骨骼动画的解决方案
4. AI可以考虑使用比较新的StateTree和SmartObject系统。Demo阶段不用考虑。
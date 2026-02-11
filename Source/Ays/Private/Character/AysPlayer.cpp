// Copyright Ayy3


#include "Public/Character/AysPlayer.h"

#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Component/SwayComponent.h"
#include "Component/TraversalComponent.h"
#include "Component/WeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/AysPlayerState.h"


AAysPlayer::AAysPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// TppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("TppSkeletalMesh");
	// TppSkeletalMesh->SetupAttachment(GetCapsuleComponent());
	// TppSkeletalMesh = GetMesh();

	FppPivot = CreateDefaultSubobject<USceneComponent>("FppPivot");
	FppPivot->SetupAttachment(GetCapsuleComponent());

	FppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FppSkeletalMesh");
	FppSkeletalMesh->SetupAttachment(GetCapsuleComponent());

	FppCamera = CreateDefaultSubobject<UCameraComponent>("FppCamera");
	FppCamera->SetupAttachment(FppSkeletalMesh, FppCameraSocketName);
	FppCamera->SetActive(true);
	// FppCamera本身会受骨骼的Roll旋转影响
	FppCamera->bUsePawnControlRotation = false;

	FppGunSceneComp = CreateDefaultSubobject<USceneComponent>("FppGunSceneComp");
	// TODO：这里应该绑到IK_Hand_Gun上，但是IK骨骼不会跟随QE变换，后续会尝试解决
	FppGunSceneComp->SetupAttachment(FppSkeletalMesh, RightHandBoneName);

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>("WeaponComponent");

	TraversalComponent = CreateDefaultSubobject<UTraversalComponent>("TraversalComponent");

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

bool AAysPlayer::TryTraversal()
{
	FTraversalCheckInputs Inputs;
	Inputs.TraceForwardDirection = GetActorForwardVector();
	Inputs.TraceForwardDistance = 200.f;
	Inputs.TraceOriginOffset = FVector();
	Inputs.TraceEndOffset = FVector();
	Inputs.TraceRadius = 30.f;
	Inputs.TraceHalfHeight = 60.f;

	bool TraversalCheckFailed = false;
	bool MontageSelectionFailed = false;
	TraversalComponent->TryTraversalAction(Inputs, TraversalCheckFailed, MontageSelectionFailed);
	return !(TraversalCheckFailed || MontageSelectionFailed);
}

UAbilitySystemComponent* AAysPlayer::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Server only
// PC and PS valid
void AAysPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent == nullptr)
	{
		if (AAysPlayerState* PS = GetPlayerState<AAysPlayerState>())
		{
			AbilitySystemComponent = Cast<UAysAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			AttributeSet = PS->AttributeSet;
			// Server端的InitAbilityActorInfo，OwnerActor为PlayerState，AvatarActor为Character
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
	}

	// 在BeginPlay调用Init会在Server端导致LocomotionComp为Nullptr（因为需要PC有效才能Retrieve）
	if (UFPSCharacterMovementComponent* CMC = Cast<UFPSCharacterMovementComponent>(GetCharacterMovement()))
	{
		CMC->InitLocomotionComponent();
		CMC->InitBasicLocomotion();
	}

	WeaponComponent->InitWeaponComponent();
}

// Client only
// PS valid
void AAysPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent == nullptr)
	{
		if (AAysPlayerState* PS = GetPlayerState<AAysPlayerState>())
		{
			AbilitySystemComponent = Cast<UAysAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			AttributeSet = PS->AttributeSet;
			// Server端的InitAbilityActorInfo，OwnerActor为PlayerState，AvatarActor为Character
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
	}
	else
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}

	WeaponComponent->InitWeaponComponent();
}



// Client only
// PC valid
void AAysPlayer::OnRep_Controller()
{
	Super::OnRep_Controller();
	// Needed in case the PC wasn't valid when we Init-ed the ASC.
	if (AAysPlayerState* PS = GetPlayerState<AAysPlayerState>())
	{
		if (PS->GetAbilitySystemComponent())
			PS->GetAbilitySystemComponent()->RefreshAbilityActorInfo();
	}

	// 在BeginPlay调用Init“我感觉有可能，但在开发的时候没有”会在Client端导致LocomotionComp为Nullptr（因为需要PC有效才能Retrieve）
	// 反正放这里准没错
	if (UFPSCharacterMovementComponent* CMC = Cast<UFPSCharacterMovementComponent>(GetCharacterMovement()))
	{
		CMC->InitLocomotionComponent();
		CMC->InitBasicLocomotion();
	}

}

void AAysPlayer::ReconstructFppCompHierarchy()
{
	if (IsValid(FppSkeletalMesh) && IsValid(FppCamera))
	{
		const FTransform SocketTransform = FppSkeletalMesh->GetSocketTransform(FppCameraSocketName);

		// 调整Pivot 位置和旋转
		FppPivot->SetWorldLocation(SocketTransform.GetLocation());
		FppPivot->SetWorldRotation(FRotator(0, GetActorRotation().Yaw, 0));

		// 重新调整Fpp的Hierarchy: FppPivot -> FppSkeletalMesh -> FppCamera
		FppSkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FppSkeletalMesh->AttachToComponent(FppPivot, FAttachmentTransformRules::KeepWorldTransform);

		// 记录初始默认的 Pivot 的 Z
		DefaultFppPivotZ = FppPivot->GetRelativeLocation().Z;

		const FTransform IKHandGunTransform = FppSkeletalMesh->GetSocketTransform(IKHandGunBoneName);
		FppGunSceneComp->SetWorldTransform(IKHandGunTransform);
	}
}

bool AAysPlayer::CanJumpInternal_Implementation() const
{
	if (Super::CanJumpInternal_Implementation())
	{
		return true;
	}

	// 仅绕过 crouch 限制，仍尊重其它跳跃条件
	if (bIsCrouched && GetMovementComponent())
	{
		UFPSCharacterMovementComponent* FPSCMC = CastChecked<UFPSCharacterMovementComponent>(GetMovementComponent());
		return FPSCMC->CanAttemptJump();
	}

	return false;
}

FCollisionQueryParams AAysPlayer::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

void AAysPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 重新调整Fpp的Hierarchy: FppPivot -> FppSkeletalMesh -> FppCamera
	ReconstructFppCompHierarchy();
}

void AAysPlayer::UpdateFppCameraTransform()
{
	if (!IsValid(FppCamera) || !IsValid(FppSkeletalMesh)) return;

	const FRotator BoneWorldRot = FppSkeletalMesh->GetSocketRotation(FppCameraSocketName);
	const float BoneRoll = BoneWorldRot.Roll;

	FRotator FinalRot = FppCamera->GetRelativeRotation();
	FinalRot.Roll = BoneRoll;

	UE_LOG(LogTemp, Warning, TEXT("AAysPlayer::UpdateFppCameraTransform BoneRoll: %f"), BoneRoll);

	FppCamera->SetRelativeRotation(FinalRot);
}

void AAysPlayer::InterpFppPivotZ(float DeltaTime)
{
	// 1. 如果 Offset 不为 0，就进行插值 (InterpSpeed 越大越快，15.0f 手感比较好)
	if (!FMath::IsNearlyZero(CurrentCrouchOffset))
	{
		CurrentCrouchOffset = FMath::FInterpTo(CurrentCrouchOffset, 0.0f, DeltaTime, CrouchInterpSpeed);
        
		// 2. 应用给 FppPivot
		if (FppSkeletalMesh)
		{
			FVector NewLoc = FppPivot->GetRelativeLocation();
			// 核心公式：位置 = 默认位置 + 当前补偿
			NewLoc.Z = DefaultFppPivotZ + CurrentCrouchOffset;
			FppPivot->SetRelativeLocation(NewLoc);
		}
	}
}

void AAysPlayer::UpdatePivotPitch()
{
	if (!IsValid(FppPivot)) return;
	// 只需要本地控制的角色更新
	if (!IsLocallyControlled()) return;
	
	FRotator PivotRot = FppPivot->GetRelativeRotation();
	PivotRot.Pitch = GetControlRotation().Pitch;
	PivotRot.Roll = 0.f;
	FppPivot->SetRelativeRotation(PivotRot);

	// 全量对齐控制器旋转，抵消胶囊体旋转带来的影响
	FppPivot->SetWorldRotation(GetControlRotation());

}

void AAysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: UE先执行Actor Tick再执行Skeletal Mesh Update，因此该相机处于上一帧的位置，需要优化
	// Head骨骼的Roll影响到FppCamera
	// UpdateFppCameraTransform();

	// 控制器旋转的Pitch影响到FppPivot
	UpdatePivotPitch();

	// 下蹲插值
	InterpFppPivotZ(DeltaTime);
}

void AAysPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAysPlayer::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	CurrentCrouchOffset += HalfHeightAdjust;
}

void AAysPlayer::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	CurrentCrouchOffset -= HalfHeightAdjust;
}


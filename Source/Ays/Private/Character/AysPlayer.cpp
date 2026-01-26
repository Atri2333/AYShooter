// Copyright Ayy3


#include "Public/Character/AysPlayer.h"

#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/AysPlayerState.h"


AAysPlayer::AAysPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	TppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("TppSkeletalMesh");
	TppSkeletalMesh->SetupAttachment(GetCapsuleComponent());

	FppPivot = CreateDefaultSubobject<USceneComponent>("FppPivot");
	FppPivot->SetupAttachment(GetCapsuleComponent());

	FppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FppSkeletalMesh");
	FppSkeletalMesh->SetupAttachment(GetCapsuleComponent());

	FppCamera = CreateDefaultSubobject<UCameraComponent>("FppCamera");
	FppCamera->SetupAttachment(FppSkeletalMesh, FppCameraSocketName);
	FppCamera->SetActive(true);
	// FppCamera本身会受骨骼的Roll旋转影响
	FppCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
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
	}
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
}

void AAysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: UE先执行Actor Tick再执行Skeletal Mesh Update，因此该相机处于上一帧的位置，需要优化
	// Head骨骼的Roll影响到FppCamera
	UpdateFppCameraTransform();

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


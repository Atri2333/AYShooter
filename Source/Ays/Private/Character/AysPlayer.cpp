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
}

void AAysPlayer::InitVariables()
{
	if (FppSkeletalMesh)
	{
		DefaultFppMeshZ = FppSkeletalMesh->GetRelativeLocation().Z;
	}
}

void AAysPlayer::BeginPlay()
{
	Super::BeginPlay();

	InitVariables();
}

void AAysPlayer::UpdateFppCameraTransform()
{
	if (!IsValid(FppCamera) || !IsValid(FppSkeletalMesh)) return;

	const FRotator BoneWorldRot = FppSkeletalMesh->GetSocketRotation(FppCameraSocketName);
	const float BoneRoll = BoneWorldRot.Roll;

	const FRotator ControllerRot = GetControlRotation();

	FRotator FinalRot;
	FinalRot.Roll = BoneRoll;
	FinalRot.Pitch = ControllerRot.Pitch;
	FinalRot.Yaw = ControllerRot.Yaw;

	FppCamera->SetWorldRotation(FinalRot);

	// const FVector HeadLocation = FppSkeletalMesh->GetSocketLocation(FppCameraSocketName);
	// FppCamera->SetWorldLocation(HeadLocation);
}

void AAysPlayer::InterpFppMeshZ(float DeltaTime)
{
	// 1. 如果 Offset 不为 0，就进行插值 (InterpSpeed 越大越快，15.0f 手感比较好)
	if (!FMath::IsNearlyZero(CurrentCrouchOffset))
	{
		CurrentCrouchOffset = FMath::FInterpTo(CurrentCrouchOffset, 0.0f, DeltaTime, CrouchInterpSpeed);
        
		// 2. 应用给 FppMesh
		if (FppSkeletalMesh)
		{
			FVector NewLoc = FppSkeletalMesh->GetRelativeLocation();
			// 核心公式：位置 = 默认位置 + 当前补偿
			NewLoc.Z = DefaultFppMeshZ + CurrentCrouchOffset;
			FppSkeletalMesh->SetRelativeLocation(NewLoc);
		}
	}
}

void AAysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: UE先执行Actor Tick再执行Skeletal Mesh Update，因此该相机处于上一帧的位置，需要优化
	UpdateFppCameraTransform();

	InterpFppMeshZ(DeltaTime);
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


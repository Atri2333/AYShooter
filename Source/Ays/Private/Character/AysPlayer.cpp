// Copyright Ayy3


#include "Public/Character/AysPlayer.h"

#include "Camera/CameraComponent.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


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

void AAysPlayer::BeginPlay()
{
	Super::BeginPlay();

	
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

void AAysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: UE先执行Actor Tick再执行Skeletal Mesh Update，因此该相机处于上一帧的位置，需要优化
	UpdateFppCameraTransform();
}

void AAysPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


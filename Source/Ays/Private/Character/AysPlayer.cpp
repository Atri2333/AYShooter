// Copyright Ayy3


#include "Public/Character/AysPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


AAysPlayer::AAysPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	TppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("TppSkeletalMesh");
	TppSkeletalMesh->SetupAttachment(GetCapsuleComponent());

	FppSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FppSkeletalMesh");
	FppSkeletalMesh->SetupAttachment(TppSkeletalMesh);

	FppCamera = CreateDefaultSubobject<UCameraComponent>("FppCamera");
	FppCamera->SetupAttachment(GetCapsuleComponent());
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

	const FVector HeadLocation = FppSkeletalMesh->GetSocketLocation(FppCameraSocketName);
	FppCamera->SetWorldLocation(HeadLocation);
}

void AAysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFppCameraTransform();
}

void AAysPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Copyright Ayy3


#include "Component/SwayComponent.h"


USwayComponent::USwayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USwayComponent::InitializeSway(USceneComponent* InPivot, USceneComponent* InMesh)
{
	PivotComp = InPivot;
	MeshComp = InMesh;
	if (MeshComp)
	{
		DefaultMeshRelRot = MeshComp->GetRelativeRotation();
	}

	if (PivotComp)
	{
		LastPivotRot = PivotComp->GetRelativeRotation();
		LastPivotQuat = LastPivotRot.Quaternion();
	}
}


void USwayComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		LastControlQuat = Pawn->GetControlRotation().Quaternion();
	}
}


void USwayComponent::UpdateSway(float DeltaTime)
{
	if (!bEnabled || !IsValid(MeshComp)) return;

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	const FQuat CurrControlQuat = Pawn->GetControlRotation().Quaternion();
	const FQuat DeltaQuat = CurrControlQuat * LastControlQuat.Inverse();
	LastControlQuat = CurrControlQuat;

	FRotator ControlDelta = DeltaQuat.Rotator();
	ControlDelta.Pitch = 0.0f;
	ControlDelta.Roll = 0.0f;

	LagOffset -= ControlDelta * LagStrength;
	LagOffset = FMath::RInterpTo(LagOffset, FRotator::ZeroRotator, DeltaTime, LagReturnSpeed);
	MeshComp->SetRelativeRotation(DefaultMeshRelRot + LagOffset);
}

void USwayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateSway(DeltaTime);

}

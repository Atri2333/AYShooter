// Copyright Ayy3


#include "Component/FPSCharacterMovementComponent.h"

#include "AysGameplayTags.h"
#include "Character/AysPlayer.h"
#include "Component/LocomotionStateComponent.h"
#include "GameFramework/Character.h"
#include "Player/AysPlayerController.h"


void UFPSCharacterMovementComponent::InitLocomotionComponent()
{
	AAysPlayer* Player = Cast<AAysPlayer>(CharacterOwner);
	if (IsValid(Player))
	{
		AAysPlayerController* PC = Cast<AAysPlayerController>(Player->GetController());
		if (IsValid(PC))
		{
			LocomotionStateComponent = PC->LocomotionStateComponent;
		}
	}
}

void UFPSCharacterMovementComponent::InitBasicLocomotion()
{
	MaxWalkSpeed = WalkSpeed;
	LocomotionStateComponent->OnLocomotionStateChanged.AddUObject(this, &ThisClass::HandleStateChange);
}

void UFPSCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	InitLocomotionComponent();
	
	InitBasicLocomotion();

	
}

void UFPSCharacterMovementComponent::InterpCrouchAlpha(float DeltaTime)
{
	const float TargetAlpha = IsCrouching() ? 1.f : 0.f;

	if (!FMath::IsNearlyEqual(CrouchAlpha, TargetAlpha))
	{
		CrouchAlpha = FMath::FInterpTo(CrouchAlpha, TargetAlpha, DeltaTime, CrouchTransitionSpeed);
	}
}

void UFPSCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	InterpCrouchAlpha(DeltaTime);
}

void UFPSCharacterMovementComponent::HandleStateChange(const FGameplayTag& Tag, bool bAdded)
{
	const FAysGameplayTags& Tags = FAysGameplayTags::Get();
	if (Tag == Tags.Action_Jump && bAdded)
	{
		if (IsValid(CharacterOwner))
		{
			CharacterOwner->Jump();
		}
	}
	else if (Tag == Tags.State_Locomotion_Sprint)
	{
		if (bAdded)
		{
			MaxWalkSpeed = RunSpeed;
		}
		else
		{
			MaxWalkSpeed = WalkSpeed;
		}
	}
	else if (Tag == Tags.State_Locomotion_Crouch)
	{
		if (bAdded)
		{
			if (IsValid(CharacterOwner))
			{
				CharacterOwner->Crouch();
			}
		}
		else
		{
			if (IsValid(CharacterOwner))
			{
				CharacterOwner->UnCrouch();
			}
		}
	}
}

// Copyright Ayy3


#include "Component/FPSCharacterMovementComponent.h"

#include "AysGameplayTags.h"
#include "Character/AysPlayer.h"
#include "Component/LocomotionStateComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Player/AysPlayerController.h"


UFPSCharacterMovementComponent::FSavedMove_FPS::FSavedMove_FPS()
{
	Saved_bWantsToSprint = false;
}

bool UFPSCharacterMovementComponent::FSavedMove_FPS::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                    ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_FPS* NewFPSMove = static_cast<const FSavedMove_FPS*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewFPSMove->Saved_bWantsToSprint)
	{
		// Sprint状态不同，不能合并
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = false;
}

uint8 UFPSCharacterMovementComponent::FSavedMove_FPS::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bWantsToSprint)
	{
		// Custom flag 0 represents sprinting
		Result |= FLAG_Custom_0;
	}
	return Result;
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::SetMoveFor(ACharacter* Character, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	if (IsValid(Character))
	{
		if (UFPSCharacterMovementComponent* FPSCMC = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			Saved_bWantsToSprint = FPSCMC->bWantsToSprint;
		}
	}
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	if (IsValid(Character))
	{
		if (UFPSCharacterMovementComponent* FPSCMC = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			FPSCMC->bWantsToSprint = Saved_bWantsToSprint;
		}
	}
}

UFPSCharacterMovementComponent::FNetworkPredictionData_Client_FPS::FNetworkPredictionData_Client_FPS(
	const UCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr UFPSCharacterMovementComponent::FNetworkPredictionData_Client_FPS::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_FPS());
}

void UFPSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_FPS::FLAG_Custom_0) != 0;
}

void UFPSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
}

FNetworkPredictionData_Client* UFPSCharacterMovementComponent::GetPredictionData_Client() const
{
	// UE内部也用了const_cast
	if (ClientPredictionData == nullptr)
	{
		UFPSCharacterMovementComponent* MutableThis = const_cast<UFPSCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_FPS(*this);
	}

	return ClientPredictionData;
}

void UFPSCharacterMovementComponent::InitLocomotionComponent()
{
	AAysPlayer* Player = Cast<AAysPlayer>(CharacterOwner);
	if (IsValid(Player))
	{
		// LocomotionStateComp在PC上，因此需要PC有效才能Retrieve出LocomotionStateComp
		// 因此需要在PC有效的时候调用本函数
		AAysPlayerController* PC = Cast<AAysPlayerController>(Player->GetController());
		if (IsValid(PC))
		{
			LocomotionStateComponent = PC->LocomotionStateComponent;
		}
	}
}

void UFPSCharacterMovementComponent::InitBasicLocomotion()
{
	MaxWalkSpeed = Walk_MaxWalkSpeed;
	if (LocomotionStateComponent != nullptr)
		LocomotionStateComponent->OnLocomotionStateChanged.AddUObject(this, &ThisClass::HandleStateChange);
}

void UFPSCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
			// 会通过SetMoveFor同步到SavedMove里传给Server
			bWantsToSprint = true;
		}
		else
		{
			bWantsToSprint = false;
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
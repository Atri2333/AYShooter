// Copyright Ayy3


#include "Player/AysPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AysAbilityInputID.h"
#include "AysGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/LocomotionStateComponent.h"
#include "GameFramework/Character.h"


AAysPlayerController::AAysPlayerController()
{
	LocomotionStateComponent = CreateDefaultSubobject<ULocomotionStateComponent>("LocomotionStateComponent");
}

void AAysPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Fpp上下视角角度限制
	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = ViewPitchMin;
		PlayerCameraManager->ViewPitchMax = ViewPitchMax;
	}

	if (const ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			// 绑定IMC
			Subsystem->AddMappingContext(BasicInputMappingContext, 0);
			Subsystem->AddMappingContext(CombatInputMappingContext, 0);
		}
	}
}

void AAysPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 默认使用EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		}
		if (LookAction)
		{
			EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		}
		if (JumpAction)
		{
			// Press 时跳跃，Released 时停止跳跃
			EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		}
		if (SprintAction)
		{
			EnhancedInputComp->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::SprintStart);
			EnhancedInputComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::SprintEnd);
		}
		if (CrouchAction)
		{
			EnhancedInputComp->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchStart);
			EnhancedInputComp->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::CrouchEnd);
		}
		if (LeanLeftAction)
		{
			EnhancedInputComp->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &ThisClass::LeanLeftStart);
			EnhancedInputComp->BindAction(LeanLeftAction, ETriggerEvent::Completed, this, &ThisClass::LeanLeftEnd);
		}
		if (LeanRightAction)
		{
			EnhancedInputComp->BindAction(LeanRightAction, ETriggerEvent::Started, this, &ThisClass::LeanRightStart);
			EnhancedInputComp->BindAction(LeanRightAction, ETriggerEvent::Completed, this, &ThisClass::LeanRightEnd);
		}
		
		if (AimAction)
		{
			EnhancedInputComp->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::AimToggle);
		}
		if (FireAction)
		{
			EnhancedInputComp->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireStart);
			EnhancedInputComp->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireEnd);
		}
		if (ReloadAction)
		{
			EnhancedInputComp->BindAction(ReloadAction, ETriggerEvent::Started, this, &ThisClass::Reload);
		}
	}
}

namespace
{
	static void ConvertCameraLocalRecoilToWorldDelta(const FRotator& CameraRot, float PitchInput, float YawInput, float& OutPitch, float& OutYaw)
	{
		const FRotator LocalDeltaRot(PitchInput, YawInput, 0.0f);
		const FQuat WorldQuat = FQuat(CameraRot) * FQuat(LocalDeltaRot);
		const FRotator WorldRot = WorldQuat.Rotator();
		const FRotator DeltaRot = (WorldRot - CameraRot).GetNormalized();

		OutPitch = DeltaRot.Pitch;
		OutYaw = DeltaRot.Yaw;
	}
}

void AAysPlayerController::SetRecoilInput(float PitchInput, float YawInput)
{
	if (FMath::IsNearlyZero(PitchInput) && FMath::IsNearlyZero(YawInput))
	{
		return;
	}

	const FRotator CameraRot = PlayerCameraManager ? PlayerCameraManager->GetCameraRotation() : GetControlRotation();
	float OutPitch = 0.0f;
	float OutYaw = 0.0f;
	ConvertCameraLocalRecoilToWorldDelta(CameraRot, PitchInput, YawInput, OutPitch, OutYaw);

	ApplyRecoilOnce(OutPitch, OutYaw);
}

void AAysPlayerController::ApplyRecoilOnce_Implementation(float PitchInput, float YawInput)
{
	if (FMath::IsNearlyZero(PitchInput) && FMath::IsNearlyZero(YawInput))
	{
		return;
	}

	AddPitchInput(PitchInput);
	AddYawInput(YawInput);
}

void AAysPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D AxisValue = Value.Get<FVector2D>();
		if (AxisValue.SizeSquared() > 0.0f)
		{
			const FVector Forward = ControlledPawn->GetActorForwardVector();
			const FVector Right = ControlledPawn->GetActorRightVector();

			ControlledPawn->AddMovementInput(Forward, AxisValue.Y);
			ControlledPawn->AddMovementInput(Right, AxisValue.X);
		}
	}
}

void AAysPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);
}

void AAysPlayerController::Jump(const FInputActionValue& Value)
{
	LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().Action_Jump);
}

void AAysPlayerController::SprintStart(const FInputActionValue& Value)
{
	LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().State_Locomotion_Sprint);
	LocomotionStateComponent->SetHoldingSprintKey(true);
}

void AAysPlayerController::SprintEnd(const FInputActionValue& Value)
{
	LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Locomotion_Sprint);
	LocomotionStateComponent->SetHoldingSprintKey(false);
}

void AAysPlayerController::CrouchStart(const FInputActionValue& Value)
{
	LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().State_Locomotion_Crouch);
}

void AAysPlayerController::CrouchEnd(const FInputActionValue& Value)
{
	LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Locomotion_Crouch);
}

void AAysPlayerController::LeanLeftStart(const FInputActionValue& Value)
{
	LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().State_Locomotion_LeanLeft);
}

void AAysPlayerController::LeanLeftEnd(const FInputActionValue& Value)
{
	LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Locomotion_LeanLeft);
}

void AAysPlayerController::LeanRightStart(const FInputActionValue& Value)
{
	LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().State_Locomotion_LeanRight);
}

void AAysPlayerController::LeanRightEnd(const FInputActionValue& Value)
{
	LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Locomotion_LeanRight);
}

void AAysPlayerController::AimToggle(const FInputActionValue& Value)
{
	if (LocomotionStateComponent->HasState(FAysGameplayTags::Get().State_Combat_Aiming))
	{
		LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Combat_Aiming);
	}
	else
	{
		LocomotionStateComponent->TryAddState(FAysGameplayTags::Get().State_Combat_Aiming);
	}
}

void AAysPlayerController::FireStart(const FInputActionValue& Value)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter()))
	{
		ASC->AbilityLocalInputPressed(static_cast<int32>(EAysAbilityInputID::Fire));
	}
}

void AAysPlayerController::FireEnd(const FInputActionValue& Value)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter()))
	{
		ASC->AbilityLocalInputReleased(static_cast<int32>(EAysAbilityInputID::Fire));
	}
}

void AAysPlayerController::Reload(const FInputActionValue& Value)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter()))
	{
		ASC->AbilityLocalInputPressed(static_cast<int32>(EAysAbilityInputID::Reload));
	}
}

// Copyright Ayy3


#include "Player/AysPlayerController.h"

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

	if (const ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			// 绑定IMC
			Subsystem->AddMappingContext(BasicInputMappingContext, 0);
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
			EnhancedInputComp->BindAction(SprintAction,ETriggerEvent::Completed, this, &ThisClass::SprintEnd);
		}
	}
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
}

void AAysPlayerController::SprintEnd(const FInputActionValue& Value)
{
	LocomotionStateComponent->RemoveState(FAysGameplayTags::Get().State_Locomotion_Sprint);
}

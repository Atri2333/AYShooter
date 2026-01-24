// Copyright Ayy3


#include "Animation/FPSAnimInstance.h"

#include "Character/AysPlayer.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Player/AysPlayerController.h"


void UFPSAnimInstance::InitPtr()
{
	if (CharacterOwner && CharacterMovementComponent && CharacterPlayerController) return;
	CharacterOwner = Cast<AAysPlayer>(TryGetPawnOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterMovementComponent = Cast<UFPSCharacterMovementComponent>(CharacterOwner->GetCharacterMovement());
		CharacterPlayerController = Cast<AAysPlayerController>(CharacterOwner->GetController());
	}
}

void UFPSAnimInstance::UpdateCrouchTranslation()
{
	if (!CharacterOwner || !CharacterMovementComponent) return;

	const float CurrentAlpha = CharacterMovementComponent->CrouchAlpha;

	const float MaxHeightDiff = CharacterOwner->GetDefaultHalfHeight() - CharacterMovementComponent->GetCrouchedHalfHeight();

	CrouchTranslationZ = FMath::Lerp(0.f, -MaxHeightDiff, CurrentAlpha);
}

void UFPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	InitPtr();
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
}

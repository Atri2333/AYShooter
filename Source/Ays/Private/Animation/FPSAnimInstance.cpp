// Copyright Ayy3


#include "Animation/FPSAnimInstance.h"

#include "AysGameplayTags.h"
#include "Character/AysPlayer.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Component/LocomotionStateComponent.h"
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

void UFPSAnimInstance::BindCallbacksToLocomotionStateComponent()
{
	if (IsValid(LocomotionStateComponent)) return;
	if (!IsValid(CharacterOwner))
	{
		CharacterOwner = Cast<AAysPlayer>(TryGetPawnOwner());
		if (!IsValid(CharacterOwner)) return;
		CharacterPlayerController = Cast<AAysPlayerController>(CharacterOwner->GetController());
		if (!IsValid(CharacterPlayerController)) return;
		LocomotionStateComponent = CharacterPlayerController->LocomotionStateComponent;
	}
	else
	{
		if (!IsValid(CharacterPlayerController))
		{
			CharacterPlayerController = Cast<AAysPlayerController>(CharacterOwner->GetController());
			if (!IsValid(CharacterPlayerController)) return;
		}
		LocomotionStateComponent = CharacterPlayerController->LocomotionStateComponent;
	}

	LocomotionStateComponent->OnLocomotionStateChanged.AddUObject(this, &ThisClass::HandleLocomotionStateChanged);
}

void UFPSAnimInstance::HandleLocomotionStateChanged(const FGameplayTag& Tag, bool bAdded)
{
	const FAysGameplayTags& Tags = FAysGameplayTags::Get();
	if (Tag == Tags.State_Locomotion_Sprint)
	{
		bIsSprinting = bAdded;
	}
	else if (Tag == Tags.State_Combat_Aiming)
	{
		bIsAiming = bAdded;
	}
	else if (Tag == Tags.State_Locomotion_LeanLeft)
	{
		bIsLeaningLeft = bAdded;
	}
	else if (Tag == Tags.State_Locomotion_LeanRight)
	{
		bIsLeaningRight = bAdded;
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

void UFPSAnimInstance::InterpLeanAngle(float DeltaSeconds)
{
	// 互斥与目标值计算
	float LeanTarget = 0.f;
	if (bIsLeaningLeft && !bIsLeaningRight)
	{
		LeanTarget = -1.f;
	}
	else if (bIsLeaningRight && !bIsLeaningLeft)
	{
		LeanTarget = 1.f;
	}
	// 平滑插值到目标
	LeanAlpha = FMath::FInterpTo(LeanAlpha, LeanTarget, DeltaSeconds, LeanInterpSpeed);
	LeanAlpha = FMath::Clamp(LeanAlpha, -1.f, 1.f);

	// 映射为角度，供 ABP 用于驱动 Spine 旋转或曲线
	LeanAngle = LeanAlpha * MaxLeanAngle;
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	
	// 将回调函数绑到 LocomotionStateComponent 的委托上
	// 放Tick是为了一定要绑定成功，可能有更好的方式 TODO
	InitPtr();
	BindCallbacksToLocomotionStateComponent();

	if (IsValid(CharacterMovementComponent))
	{
		bIsInAir = CharacterMovementComponent->IsFalling();

		const FVector Velocity = CharacterMovementComponent->Velocity;
		SpeedXY = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	}

	InterpLeanAngle(DeltaSeconds);
}

// Copyright Ayy3


#include "Component/LocomotionStateComponent.h"

#include "AysGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
ULocomotionStateComponent::ULocomotionStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool ULocomotionStateComponent::IsInAir() const
{
	// 该组件与输入有关，因此Owner为Controller
	const APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	const ACharacter* Character = PlayerController->GetCharacter();
	if (Character && Character->GetCharacterMovement())
	{
		// IsFalling() 包含了 Falling (下落) 和 Jumping (跳跃上升) 两种情况
		return Character->GetCharacterMovement()->IsFalling();
	}
	return false;
}

void ULocomotionStateComponent::TryAddState(const FGameplayTag& Tag)
{
	// 互斥逻辑
	const FAysGameplayTags& Tags = FAysGameplayTags::Get();
	if (Tag == Tags.State_Locomotion_Sprint)
	{
		if (HasState(Tags.State_Locomotion_Crouch))
		{
			// 冲刺的时候，取消下蹲
			RemoveState(Tags.State_Locomotion_Crouch);
		}
		if (HasState(Tags.State_Combat_Aiming))
		{
			// 冲刺的时候取消瞄准
			RemoveState(Tags.State_Combat_Aiming);
		}
		if (HasState(Tags.State_Locomotion_LeanLeft))
		{
			// 取消左倾
			RemoveState(Tags.State_Locomotion_LeanLeft);
		}
		if (HasState(Tags.State_Locomotion_LeanRight))
		{
			// 取消右倾
			RemoveState(Tags.State_Locomotion_LeanRight);
		}
	}
	else if (Tag == Tags.State_Locomotion_Crouch)
	{
		// 不在空中下蹲
		if (IsInAir()) return;
		
		if (HasState(Tags.State_Locomotion_Sprint))
		{
			// 下蹲的时候取消冲刺
			RemoveState(Tags.State_Locomotion_Sprint);
		}
	}
	else if (Tag == Tags.State_Combat_Aiming)
	{
		if (HasState(Tags.State_Locomotion_Sprint))
		{
			// 瞄准的时候取消冲刺
			RemoveState(Tags.State_Locomotion_Sprint);
		}
	}
	else if (Tag == Tags.State_Locomotion_LeanLeft)
	{
		if (HasState(Tags.State_Locomotion_LeanRight))
		{
			// 取消右倾
			RemoveState(Tags.State_Locomotion_LeanRight);
		}
		if (HasState(Tags.State_Locomotion_Sprint))
		{
			// 探头时一律取消冲刺
			RemoveState(Tags.State_Locomotion_Sprint);
		}
	}
	else if (Tag == Tags.State_Locomotion_LeanRight)
	{
		if (HasState(Tags.State_Locomotion_LeanLeft))
		{
			// 取消左倾
			RemoveState(Tags.State_Locomotion_LeanLeft);
		}
		if (HasState(Tags.State_Locomotion_Sprint))
		{
			// 探头时一律取消冲刺
			RemoveState(Tags.State_Locomotion_Sprint);
		}
	}

	// 对于Action的Tag就不加到TagContainer里了
	if (Tag.MatchesTag(Tags.State_Locomotion) || Tag.MatchesTag(Tags.State_Combat))
	{
		CurrentStates.AddTag(Tag);
	}
	OnLocomotionStateChanged.Broadcast(Tag, true);
}

void ULocomotionStateComponent::RemoveState(const FGameplayTag& Tag)
{
	if (CurrentStates.HasTag(Tag))
	{
		CurrentStates.RemoveTag(Tag);
		OnLocomotionStateChanged.Broadcast(Tag, false);

		const bool bPreConditionForSprint = Tag != FAysGameplayTags::Get().State_Locomotion_Sprint;
		// 尝试重新冲刺
		if (bPreConditionForSprint)
			TryReactiveSprint();
	}
}

bool ULocomotionStateComponent::HasState(const FGameplayTag& Tag) const
{
	return CurrentStates.HasTagExact(Tag);
}


// Called when the game starts
void ULocomotionStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void ULocomotionStateComponent::TryReactiveSprint()
{
	if (CanReactiveSprint())
	{
		const FAysGameplayTags& Tags = FAysGameplayTags::Get();
		TryAddState(Tags.State_Locomotion_Sprint);
	}
}

// Should be Re-modified if more conditions are added, fuck my fool brain for this design
bool ULocomotionStateComponent::CanReactiveSprint() const
{
	if (!bHoldingSprintKey) return false;
	
	const FAysGameplayTags& Tags = FAysGameplayTags::Get();
	
	if (HasState(Tags.State_Combat_Aiming)) return false;
	if (HasState(Tags.State_Locomotion_Crouch)) return false;
	if (HasState(Tags.State_Locomotion_LeanLeft) || HasState(Tags.State_Locomotion_LeanRight)) return false;

	return true;
}

// Called every frame
void ULocomotionStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


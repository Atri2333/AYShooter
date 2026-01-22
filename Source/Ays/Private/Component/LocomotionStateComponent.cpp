// Copyright Ayy3


#include "Component/LocomotionStateComponent.h"

#include "AysGameplayTags.h"


// Sets default values for this component's properties
ULocomotionStateComponent::ULocomotionStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULocomotionStateComponent::TryAddState(const FGameplayTag& Tag)
{
	// 互斥逻辑
	FAysGameplayTags Tags = FAysGameplayTags::Get();
	if (Tag == Tags.State_Locomotion_Sprint)
	{
		if (HasState(Tags.State_Locomotion_Crouch))
		{
			// 冲刺的时候，取消下蹲
			RemoveState(Tags.State_Locomotion_Crouch);
		}
	}
	else if (Tag == Tags.State_Locomotion_Crouch)
	{
		if (HasState(Tags.State_Locomotion_Sprint))
		{
			// 下蹲的时候取消冲刺
			RemoveState(Tags.State_Locomotion_Sprint);
		}
	}

	
	if (Tag.MatchesTag(Tags.State_Locomotion))
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


// Called every frame
void ULocomotionStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


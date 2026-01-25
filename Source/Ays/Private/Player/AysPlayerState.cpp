// Copyright Ayy3


#include "Player/AysPlayerState.h"

#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "AbilitySystem/AysAttributeSet.h"


UAbilitySystemComponent* AAysPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

AAysPlayerState::AAysPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAysAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAysAttributeSet>("AttributeSet");

	// AbilitySystemComponent needs to be updated at a high frequency. (From Lyra)
	SetNetUpdateFrequency(100.0f);
}

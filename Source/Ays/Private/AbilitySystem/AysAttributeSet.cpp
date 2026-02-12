// Copyright Ayy3


#include "AbilitySystem/AysAttributeSet.h"

#include "Net/UnrealNetwork.h"


void UAysAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAysAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAysAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAysAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAysAttributeSet, Health, OldHealth);
}

void UAysAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAysAttributeSet, MaxHealth, OldMaxHealth);
}

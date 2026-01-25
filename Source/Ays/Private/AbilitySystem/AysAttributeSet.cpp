// Copyright Ayy3


#include "AbilitySystem/AysAttributeSet.h"

#include "Net/UnrealNetwork.h"


void UAysAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAysAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UAysAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAysAttributeSet, Health, OldHealth);
}

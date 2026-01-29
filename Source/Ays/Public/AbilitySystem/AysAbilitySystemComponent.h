// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AysAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API UAysAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	
};

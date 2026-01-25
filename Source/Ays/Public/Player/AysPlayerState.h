// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AysPlayerState.generated.h"

class UAysAttributeSet;
class UAysAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AYS_API AAysPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAysPlayerState();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAysAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAysAttributeSet> AttributeSet;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};

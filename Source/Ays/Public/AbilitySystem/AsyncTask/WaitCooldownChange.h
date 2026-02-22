// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 
 */
UCLASS()
class AYS_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;
	
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag);
	
	UFUNCTION(BlueprintCallable)
	void EndTask();
	
protected:
	
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	FGameplayTag CooldownTag;
	
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	void OnActiveEffectApplied(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};

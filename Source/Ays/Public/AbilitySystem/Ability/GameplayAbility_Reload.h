// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"
#include "GameplayAbility_Reload.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API UGameplayAbility_Reload : public UGameplayAbility_WeaponBase
{
	GENERATED_BODY()
public:
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	
protected:
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FppReloadMontage;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TppReloadMontage;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> WeaponReloadMontage;
	
	void OnReloadConfirmEvent(const FGameplayEventData* EventData);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BeginReload();
	
	UFUNCTION(BlueprintCallable)
	void ApplyReloadLogic();
	
};

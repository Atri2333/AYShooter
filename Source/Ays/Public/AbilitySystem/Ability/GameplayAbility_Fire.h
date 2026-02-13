// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"
#include "GameplayAbility_Fire.generated.h"

class AAysPlayerController;
/**
 * 
 */
UCLASS()
class AYS_API UGameplayAbility_Fire : public UGameplayAbility_WeaponBase
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Fire();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:

	UFUNCTION(BlueprintCallable)
	virtual void StartAutoFire();

	UFUNCTION(BlueprintNativeEvent)
	void DoFireOnce();

	UFUNCTION(BlueprintCallable)
	virtual void ClearAutoFire();

	UFUNCTION(Blueprintable)
	virtual void ApplyRecoilOnce();
	
	UFUNCTION(BlueprintPure)
	bool CanFire();

protected:

	FTimerHandle AutoFireTimerHandle;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FppFireMontage;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FppAimedFireMontage;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AAysPlayerController> OwnerPC;

	

	float FireInterval;

	bool bIsAutoFiring = false;

	int32 ShotsFired = 0;
};

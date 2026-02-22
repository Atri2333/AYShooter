// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Dash.generated.h"

class AAysPlayerController;
class AAysPlayer;
class UFPSCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class AYS_API UGameplayAbility_Dash : public UGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Locomotion")
	FGameplayTagContainer InvalidateLocomotionTags;
	
protected:
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UFPSCharacterMovementComponent> FPSCMC;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AAysPlayer> AysPlayer;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AAysPlayerController> AysPlayerController;
	
	
};

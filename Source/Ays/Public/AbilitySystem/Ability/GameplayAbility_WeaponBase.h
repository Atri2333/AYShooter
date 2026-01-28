// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/AysPlayer.h"
#include "GameplayAbility_WeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API UGameplayAbility_WeaponBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_WeaponBase();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 初始化指针缓存
	void InitCachedRefs(const FGameplayAbilityActorInfo* ActorInfo);

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AAysPlayer> OwnerPlayer;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAysAbilitySystemComponent> OwnerASC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> OwnerWeaponComp;
	
public:

	
	
	
};

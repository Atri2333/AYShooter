// Copyright Ayy3


#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"

#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "Component/WeaponComponent.h"


UGameplayAbility_WeaponBase::UGameplayAbility_WeaponBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_WeaponBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	InitCachedRefs(ActorInfo);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

void UGameplayAbility_WeaponBase::InitCachedRefs(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (ActorInfo == nullptr) return;

	OwnerASC = CastChecked<UAysAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	OwnerPlayer = CastChecked<AAysPlayer>(ActorInfo->AvatarActor.Get());
	OwnerWeaponComp = OwnerPlayer->GetWeaponComponent();
	WeaponDataAsset = OwnerWeaponComp->GetWeaponDataAsset();
	checkf(OwnerWeaponComp != nullptr, TEXT("UGameplayAbility_WeaponBase::InitCachedRefs: OwnerWeaponComp is nullptr!"));
	checkf(WeaponDataAsset != nullptr, TEXT("UGameplayAbility_WeaponBase::InitCachedRefs: WeaponDataAsset is nullptr!"));
}

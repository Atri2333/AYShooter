// Copyright Ayy3


#include "AbilitySystem/Ability/GameplayAbility_Reload.h"

#include "AbilitySystemComponent.h"
#include "AysGameplayTags.h"
#include "Data/WeaponDataAsset.h"


void UGameplayAbility_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FppReloadMontage = WeaponDataAsset->GetFppReloadMontageByTag(OwnerWeaponComp->GetCurrentWeaponTag());
	TppReloadMontage = WeaponDataAsset->GetTppReloadMontageByTag(OwnerWeaponComp->GetCurrentWeaponTag());
	WeaponReloadMontage = WeaponDataAsset->GetWeaponReloadMontageByTag(OwnerWeaponComp->GetCurrentWeaponTag());
	
	// 服务端校验
	if (HasAuthority(&ActivationInfo))
	{
		if (!OwnerWeaponComp->CanReloadWeapon())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		FGameplayEventData ConfirmData;
		ConfirmData.Instigator = ActorInfo->AvatarActor.Get();
		GetAbilitySystemComponentFromActorInfo()->HandleGameplayEvent(FAysGameplayTags::Get().Event_Weapon_ReloadConfirm, &ConfirmData);
	}
	else
	{
		
	}
}

void UGameplayAbility_Reload::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	if (GetAbilitySystemComponentFromActorInfo())
	{
		GetAbilitySystemComponentFromActorInfo()->GenericGameplayEventCallbacks
			.FindOrAdd(FAysGameplayTags::Get().Event_Weapon_ReloadConfirm)
			.AddUObject(this, &ThisClass::OnReloadConfirmEvent);
	}
}

// both server and client???
void UGameplayAbility_Reload::OnReloadConfirmEvent(const FGameplayEventData* EventData)
{
	BeginReload();
}

void UGameplayAbility_Reload::ApplyReloadLogic()
{
	// server only
	if (HasAuthority(&CurrentActivationInfo))
	{
		OwnerWeaponComp->ApplyReloadLogic();
	}
}
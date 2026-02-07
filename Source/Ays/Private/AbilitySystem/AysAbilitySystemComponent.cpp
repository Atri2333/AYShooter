// Copyright Ayy3


#include "AbilitySystem/AysAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "AysGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"
#include "Component/WeaponComponent.h"


void UAysAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	const auto* WeaponAbility = Cast<UGameplayAbility_WeaponBase>(AbilitySpec.Ability.Get());
	if (!WeaponAbility) return;

	// 如果是装备武器的Ability，则调用WeaponComponent的EquipInitialWeapon函数
	const FGameplayTag WeaponAbilityTag = WeaponAbility->GetAssetTags().First();
	if (WeaponAbilityTag == FAysGameplayTags::Get().Ability_Weapon_Equip)
	{
		AAysPlayer* Player = Cast<AAysPlayer>(GetAvatarActor());
		if (IsValid(Player))
		{
			Player->GetWeaponComponent()->EquipInitialWeapon();
		}
	}
}

void UAysAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

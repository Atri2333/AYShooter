// Copyright Ayy3


#include "AbilitySystem/AysAbilitySystemComponent.h"

#include "AysGameplayTags.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"
#include "Component/WeaponComponent.h"


void UAysAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	const auto* WeaponAbility = Cast<UGameplayAbility_WeaponBase>(AbilitySpec.Ability.Get());
	if (!WeaponAbility) return;

	// 如果是装备武器的Ability，则调用WeaponComponent的EquipInitialWeapon函数
	if (WeaponAbility->GetWeaponAbilityTag() == FAysGameplayTags::Get().Ability_Weapon_Equip)
	{
		AAysPlayer* Player = Cast<AAysPlayer>(GetAvatarActor());
		if (IsValid(Player))
		{
			Player->GetWeaponComponent()->EquipInitialWeapon();
		}
	}
}

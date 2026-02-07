// Copyright Ayy3


#include "Data/WeaponDataAsset.h"


FWeaponData UWeaponDataAsset::GetWeaponDataByTag(const FGameplayTag& WeaponTag) const
{
	for (const FWeaponData& WeaponData : AllWeapons)
	{
		if (WeaponData.WeaponTag == WeaponTag)
		{
			return WeaponData;
		}
	}
	
	return FWeaponData();
}

FName UWeaponDataAsset::GetWeaponNameByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.WeaponName;
}

UAnimMontage* UWeaponDataAsset::GetFppEquipMontageByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.FppEquipMontage;
}

UAnimMontage* UWeaponDataAsset::GetFppUnequipMontageByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.FppUnequipMontage;
}

UAnimMontage* UWeaponDataAsset::GetFppReloadMontageByTag(const FGameplayTag& Weapon) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(Weapon);
	return WeaponData.FppReloadMontage;
}

UAnimMontage* UWeaponDataAsset::GetFppFireMontageByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.FppFireMontage;
}

UAnimMontage* UWeaponDataAsset::GetFppAimedFireMontageByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.FppAimedFireMontage;
}

USoundBase* UWeaponDataAsset::GetFireSoundByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.FireSound;
}

UNiagaraSystem* UWeaponDataAsset::GetMuzzleFlashEffectByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.MuzzleFlashEffect;
}

UCurveVector* UWeaponDataAsset::GetRecoilPerShotCurveByTag(const FGameplayTag& WeaponTag) const
{
	const FWeaponData WeaponData = GetWeaponDataByTag(WeaponTag);
	return WeaponData.RecoilPerShotCurve;
}

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

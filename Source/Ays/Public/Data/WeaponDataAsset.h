// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/Weapon.h"
#include "WeaponDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponName;
};

/**
 * 
 */
UCLASS()
class AYS_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<FWeaponData> AllWeapons;

public:

	FWeaponData GetWeaponDataByTag(const FGameplayTag& WeaponTag) const;
	
	
};

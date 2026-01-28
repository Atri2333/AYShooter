// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/Weapon.h"
#include "WeaponDataAsset.generated.h"

class UGameplayAbility_WeaponBase;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> FppEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> FppUnequipMontage;

	// 对于近战武器，留空即可
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> FppReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> FppFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> FppAimedFireMontage;

	// TODO: Tpp动画待补充
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<UGameplayAbility_WeaponBase>> StartupWeaponAbilities;

public:

	FWeaponData GetWeaponDataByTag(const FGameplayTag& WeaponTag) const;
	FName GetWeaponNameByTag(const FGameplayTag& WeaponTag) const;
	UAnimMontage* GetFppEquipMontageByTag(const FGameplayTag& WeaponTag) const;
	UAnimMontage* GetFppUnequipMontageByTag(const FGameplayTag& WeaponTag) const;
	UAnimMontage* GetFppReloadMontageByTag(const FGameplayTag& Weapon) const;
	UAnimMontage* GetFppFireMontageByTag(const FGameplayTag& WeaponTag) const;
	UAnimMontage* GetFppAimedFireMontageByTag(const FGameplayTag& WeaponTag) const;
	
	
};

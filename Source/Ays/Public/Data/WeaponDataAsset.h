// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/Weapon.h"
#include "WeaponDataAsset.generated.h"

class UGameplayEffect;
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

	UFUNCTION(BlueprintPure)
	FWeaponData GetWeaponDataByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	FName GetWeaponNameByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetFppEquipMontageByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetFppUnequipMontageByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetFppReloadMontageByTag(const FGameplayTag& Weapon) const;
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetFppFireMontageByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetFppAimedFireMontageByTag(const FGameplayTag& WeaponTag) const;
	
	
};

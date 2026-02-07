// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AysAbilityInputID.h"
#include "Engine/DataAsset.h"
#include "Weapon/Weapon.h"
#include "WeaponDataAsset.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class UGameplayAbility_WeaponBase;
class UCurveVector;

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

	// 开火音效和粒子特效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UNiagaraSystem> MuzzleFlashEffect;

	// Per-shot recoil offsets: X=Pitch, Y=Yaw, Z=unused.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	TObjectPtr<UCurveVector> RecoilPerShotCurve;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AYS_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<FWeaponData> AllWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<UGameplayAbility_WeaponBase>> StartupWeaponAbilities;

	// 如果GA有对应的InputID，直接通过InputID来Activate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TMap<FGameplayTag, EAysAbilityInputID> AbilityToInputIDMap;

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
	UFUNCTION(BlueprintPure)
	USoundBase* GetFireSoundByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UNiagaraSystem* GetMuzzleFlashEffectByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintPure)
	UCurveVector* GetRecoilPerShotCurveByTag(const FGameplayTag& WeaponTag) const;

	
};

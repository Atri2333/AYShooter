// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"
#include "GameplayAbility_Fire.generated.h"

class AAysPlayerController;

USTRUCT(BlueprintType)
struct FFireTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector RelativeMuzzleOffset;
	
	UPROPERTY()
	FVector FireDirection;
	
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFireTargetData::StaticStruct();
	}
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << RelativeMuzzleOffset;
		Ar << FireDirection;
		
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FFireTargetData> : public TStructOpsTypeTraitsBase2<FFireTargetData>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

/**
 * 
 */
UCLASS()
class AYS_API UGameplayAbility_Fire : public UGameplayAbility_WeaponBase
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Fire();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:

	UFUNCTION(BlueprintCallable)
	virtual void StartAutoFire();

	UFUNCTION(BlueprintNativeEvent)
	void DoFireOnce();

	UFUNCTION(BlueprintCallable)
	virtual void ClearAutoFire();

	UFUNCTION(Blueprintable)
	virtual void ApplyRecoilOnce();
	
	UFUNCTION(BlueprintPure)
	bool CanFire();
	
	void DoFireTrace(const FVector& Start, const FVector& End, FHitResult& OutHitResult);

protected:

	FTimerHandle AutoFireTimerHandle;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FppFireMontage;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FppAimedFireMontage;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AAysPlayerController> OwnerPC;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MuzzleSocketName = TEXT("MuzzleFlashSocket");

	float FireInterval;

	bool bIsAutoFiring = false;

	int32 ShotsFired = 0;
	
protected:
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};

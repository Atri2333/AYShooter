// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/AysPlayer.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


class UWeaponDataAsset;
class UAysAbilitySystemComponent;
class AWeapon;

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AYS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UWeaponComponent();

	void InitWeaponComponent();
	void EquipInitialWeapon();

	void EquipWeapon(AWeapon* InWeapon);

	void SwitchWeapon(const FGameplayTag& NewWeaponTag);

	FORCEINLINE UWeaponDataAsset* GetWeaponDataAsset() const { return WeaponDataAsset; }
	FORCEINLINE AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentWeaponTag() const;

	UFUNCTION(BlueprintPure)
	FTransform GetCurrentWeaponSocketTransform(const FName& SocketName) const;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag DefaultWeaponTag;

	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UAysAbilitySystemComponent> OwnerASC;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AAysPlayer> OwnerPlayer;

	UPROPERTY(EditDefaultsOnly, Category="Data")
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;


public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", ReplicatedUsing=OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AWeapon>> Inventory;

	UPROPERTY()
	FGameplayTagContainer OwnedWeaponTags;

	UFUNCTION()
	void OnRep_CurrentWeapon();

protected:

};

// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/AysPlayer.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


struct FWeaponUIData;
class UWeaponDataAsset;
class UAysAbilitySystemComponent;
class AWeapon;

DECLARE_MULTICAST_DELEGATE_OneParam(FWeaponUIDataChangedSignature, const FWeaponUIData& /*WeaponUIData*/);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AYS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	FWeaponUIDataChangedSignature OnWeaponUIDataChanged;

	UWeaponComponent();

	void InitWeaponComponent();
	void EquipInitialWeapon();

	void EquipWeapon(AWeapon* InWeapon);

	void SwitchWeapon(const FGameplayTag& NewWeaponTag);

	void FireWeapon();
	
	bool CanFireWeapon() const;
	bool CanReloadWeapon() const;
	void ApplyReloadLogic();

	void OnWeaponStatChanged();
	void OnWeaponStatChanged(const FWeaponUIData& WeaponUIData);
	const FWeaponUIData GetWeaponUIData() const;

	FORCEINLINE UWeaponDataAsset* GetWeaponDataAsset() const { return WeaponDataAsset; }
	FORCEINLINE AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentWeaponTag() const;

	UFUNCTION(BlueprintPure)
	FTransform GetCurrentWeaponSocketTransform(const FName& SocketName) const;
	
	UFUNCTION(BlueprintPure)
	float GetCurrentWeaponFireDistance() const;
	
	UFUNCTION(BlueprintPure)
	float GetCurrentWeaponFireWidth() const;
	
	UFUNCTION(BlueprintPure)
	float CalculateMaxWeaponSpreadAngle() const;

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

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FFloatRange SpreadSpeedRange = FFloatRange(50.f, 150.f);

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
	
	static FVector VRandCone(FVector const& Dir, float ConeHalfAngleRad);

protected:

};

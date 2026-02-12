// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "GunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API AGunWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AGunWeapon();
	virtual const FWeaponUIData GetUIData() const override;

protected:

	void BeginPlay() override;

	// 弹夹容量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunWeapon")
	int32 MagazineCapacity;

	// 目前弹匣中子弹数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunWeapon")
	int32 AmmoInMag;

	// 总共剩余的弹药数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunWeapon")
	int32 TotalAmmo;
	
	

	// 弹匣Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GunWeapon")
	TObjectPtr<UStaticMeshComponent> MagazineMesh;

	// 弹匣Socket名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunWeapon")
	FName MagazineSocketName = FName("Magazine");

public:

	void Tick(float DeltaTime) override;
	
	
	
};

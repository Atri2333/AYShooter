// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/WeaponComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponFiredSignature, const FWeaponUIData&);

UCLASS()
class AYS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	// 装备武器
	virtual void OnEquipped();

	FORCEINLINE FName GetFppEquipSocketName() const { return FppEquipSocketName; }
	FORCEINLINE bool CanRepeatAttack() const { return bCanRepeatAttack; }
	FORCEINLINE float GetFireRate() const { return FireRate; }
	FTransform GetSocketTransform(FName SocketName) const;
	virtual const FWeaponUIData GetUIData() const;
	virtual void FireLogic();
	virtual bool CanFire() const;
	virtual bool CanReload() const;
	virtual void ApplyReloadLogic();
	virtual float GetFireDistance() const { return AttackRange; }
	virtual float GetAttackWidth() const {return AttackWidth;}
	virtual float GetMaxSpreadAngle() const { return MaxSpreadAngle; }

	FOnWeaponFiredSignature OnWeaponFiredDelegate;

protected:
	
	virtual void BeginPlay() override;

	// 单次攻击伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage;
	
	// 攻击范围
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AttackRange = 1000.f;
	
	// 攻击宽度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AttackWidth = 10.f;

	// 是否可以重复攻击（如自动步枪）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bCanRepeatAttack;

	// 开火速率（每秒发射子弹数）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate;
	
	// 武器Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// 武器装备到角色类上的Fpp Socket名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName FppEquipSocketName;

	// 最大散步角度（做开火时的弹道散布）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float MaxSpreadAngle = 5.f;

public:
	
	virtual void Tick(float DeltaTime) override;

	// 武器对应GameplayTag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;
	
};

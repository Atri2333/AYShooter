// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

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

protected:
	
	virtual void BeginPlay() override;

	// 单次攻击伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage;

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
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	FName FppEquipSocketName;


public:
	
	virtual void Tick(float DeltaTime) override;

	// 武器对应GameplayTag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;
	
};

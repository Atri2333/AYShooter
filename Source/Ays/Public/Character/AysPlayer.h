// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AysPlayer.generated.h"

class UTraversalComponent;
class USwayComponent;
class UWeaponComponent;
class UAysAttributeSet;
class UAysAbilitySystemComponent;
class UFPSCharacterMovementComponent;
class UCameraComponent;

UCLASS()
class AYS_API AAysPlayer : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAysPlayer(const FObjectInitializer& ObjectInitializer);

	bool TryTraversal();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	FORCEINLINE USkeletalMeshComponent* GetFppMesh() const { return FppSkeletalMesh; }
	FORCEINLINE USceneComponent* GetFppGunSceneComp() const { return FppGunSceneComp; }
	FORCEINLINE UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	FCollisionQueryParams GetIgnoreCharacterParams() const;

protected:
	virtual void BeginPlay() override;

	void UpdateFppCameraTransform();
	void InterpFppPivotZ(float DeltaTime);
	void UpdatePivotPitch();
	void ReconstructFppCompHierarchy();

	virtual bool CanJumpInternal_Implementation() const override;

	// 记录 FPP Pivot 的初始相对位置 (Z轴)
	float DefaultFppPivotZ = 0.0f;

	// 当前的视觉补偿偏移量 (我们会不断把这个值插值回 0)
	float CurrentCrouchOffset = 0.0f;

	UPROPERTY(EditAnywhere, Category="Crouch")
	float CrouchInterpSpeed = 15.f;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// FppPivot，用于旋转第一人称视角骨骼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPP")
	TObjectPtr<USceneComponent> FppPivot;
	
	// 第一人称视角相机，绑定到骨骼head上
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FppCamera;

	// Gun SceneComp
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPP")
	TObjectPtr<USceneComponent> FppGunSceneComp;

	// 武器组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UWeaponComponent> WeaponComponent;

	// Traversal组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<UTraversalComponent> TraversalComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FName FppCameraSocketName = FName("HeadSocket");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName RightHandBoneName = FName("hand_r");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName IKHandGunBoneName = FName("ik_hand_gun");

	// 第一人称视角骨骼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FppRig")
	TObjectPtr<USkeletalMeshComponent> FppSkeletalMesh;

	// 第三人称视角骨骼（完整）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TppRig")
	TObjectPtr<USkeletalMeshComponent> TppSkeletalMesh;

	// ASC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAysAbilitySystemComponent> AbilitySystemComponent;

	// AS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAysAttributeSet> AttributeSet;

private:
	void InitAbilityActorInfo();
};

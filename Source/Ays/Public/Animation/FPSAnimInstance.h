// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSAnimInstance.generated.h"

struct FGameplayTag;
class ULocomotionStateComponent;
class AAysPlayerController;
class UFPSCharacterMovementComponent;
class AAysPlayer;
/**
 * 
 */
UCLASS()
class AYS_API UFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 初始化指针，但事实上你把握不住那些类的初始化顺序
	void InitPtr();

	void BindCallbacksToLocomotionStateComponent();

	void HandleLocomotionStateChanged(const FGameplayTag& Tag, bool bAdded);
	

	UPROPERTY(BlueprintReadOnly, Category = "Deprecated")
	float CrouchTranslationZ = 0.f;

	// ABP Controlled Crouch, Deprecated
	void UpdateCrouchTranslation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Locomotion")
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Locomotion")
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Locomotion")
	bool bIsLeaningLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Locomotion")
	bool bIsLeaningRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Combat")
	bool bIsAiming;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property|Locomotion")
	float SpeedXY;

	UPROPERTY()
	TObjectPtr<AAysPlayer> CharacterOwner;

	UPROPERTY()
	TObjectPtr<UFPSCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	TObjectPtr<ULocomotionStateComponent> LocomotionStateComponent;

	UPROPERTY()
	TObjectPtr<AAysPlayerController> CharacterPlayerController;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property|Locomotion")
	float LeanAlpha = 0.f;

	// 最大倾斜角度（度），ABP 可直接驱动旋转或曲线
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property|Locomotion")
	float MaxLeanAngle = 12.f;

	// 倾斜插值速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property|Locomotion")
	float LeanInterpSpeed = 10.f;

	// 由 LeanAlpha 映射出的角度值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property|Locomotion")
	float LeanAngle = 0.f;

	// 插值 LeanAngle
	void InterpLeanAngle(float DeltaSeconds);
};

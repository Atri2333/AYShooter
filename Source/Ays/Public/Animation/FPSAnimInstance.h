// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSAnimInstance.generated.h"

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
	// 初始化指针
	void InitPtr();
	

	UPROPERTY(BlueprintReadOnly, Category = "Deprecated")
	float CrouchTranslationZ = 0.f;

	// ABP Controlled Crouch, Deprecated
	void UpdateCrouchTranslation();

	UPROPERTY()
	TObjectPtr<AAysPlayer> CharacterOwner;

	UPROPERTY()
	TObjectPtr<UFPSCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	TObjectPtr<AAysPlayerController> CharacterPlayerController;
	
	
};

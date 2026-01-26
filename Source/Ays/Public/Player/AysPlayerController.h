// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AysPlayerController.generated.h"

class ULocomotionStateComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class AYS_API AAysPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAysPlayerController();
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	// Locomotion State Comp
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULocomotionStateComponent> LocomotionStateComponent;
	
protected:

	// 基础的IMC，负责基础的人物交互
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BasicInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	// Combat相关IMC，例如开火、瞄准等
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> CombatInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;

	// 输入处理函数
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintEnd(const FInputActionValue& Value);
	void CrouchStart(const FInputActionValue& Value);
	void CrouchEnd(const FInputActionValue& Value);
	void AimToggle(const FInputActionValue& Value);
};

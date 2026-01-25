// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovementComponent.generated.h"

class ULocomotionStateComponent;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class AYS_API UFPSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	void InitLocomotionComponent();
	void InitBasicLocomotion();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InterpCrouchAlpha(float DeltaTime);
	void HandleStateChange(const FGameplayTag& Tag, bool bAdded);

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	float RunSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Deprecared")
	float CrouchAlpha = 0.f;

	UPROPERTY(EditAnywhere, Category = "Deprecared")
	float CrouchTransitionSpeed = 10.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULocomotionStateComponent> LocomotionStateComponent;
};

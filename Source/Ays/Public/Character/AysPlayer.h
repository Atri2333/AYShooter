// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AysPlayer.generated.h"

class UCameraComponent;

UCLASS()
class AYS_API AAysPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AAysPlayer();


protected:
	virtual void BeginPlay() override;

	void UpdateFppCameraTransform();
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 第一人称视角相机，绑定到骨骼head上
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FppCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FName FppCameraSocketName = FName("HeadSocket");

	// 第一人称视角骨骼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FppRig")
	TObjectPtr<USkeletalMeshComponent> FppSkeletalMesh;

	// 第三人称视角骨骼（完整）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TppRig")
	TObjectPtr<USkeletalMeshComponent> TppSkeletalMesh;
	
};

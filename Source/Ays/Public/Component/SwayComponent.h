// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType )
class AYS_API USwayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USwayComponent();
	
	void InitializeSway(USceneComponent* InPivot, USceneComponent* InMesh);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HandSway")
	bool bEnabled = true;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HandSway")
	float InterpSpeed =12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HandSway")
	float LagStrength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HandSway")
	float LagReturnSpeed = 8.0f;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> PivotComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MeshComp;

	void UpdateSway(float DeltaTime);
	
	FRotator DefaultMeshRelRot = FRotator::ZeroRotator;
	FRotator LastPivotRot = FRotator::ZeroRotator;
	FQuat LastPivotQuat = FQuat::Identity;
	FQuat LastControlQuat = FQuat::Identity;
	FRotator LagOffset = FRotator::ZeroRotator;
	

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	
};

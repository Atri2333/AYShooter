// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TraversalSpline.generated.h"

class UBoxComponent;
class USplineComponent;

UCLASS()
class AYS_API ATraversalSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	ATraversalSpline();
	FORCEINLINE ATraversalSpline* GetOppositeTraversal() const { return OppositeTraversal; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<ATraversalSpline> OppositeTraversal;

	// 动态给TraversalComponent添加Spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<UBoxComponent> TriggerVolume;

protected:
	
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

	
	
};

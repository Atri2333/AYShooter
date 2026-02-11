// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actor/TraversalSpline.h"
#include "TraversalComponent.generated.h"

class UCapsuleComponent;

USTRUCT(BlueprintType)
struct FTraversalCheckInputs
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector TraceForwardDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float TraceForwardDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector TraceOriginOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector TraceEndOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float TraceRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float TraceHalfHeight;
};

USTRUCT(BlueprintType)
struct FCharacterPropertiesForTraversal
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<USkeletalMeshComponent> FppMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<USkeletalMeshComponent> TppMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float Speed;
};

UENUM(BlueprintType)
enum ETraversalActionType : uint8
{
	ETAT_None UMETA(DisplayName = "None"),
	ETAT_Hurdle UMETA(DisplayName = "Hurdle"),
	ETAT_Vault UMETA(DisplayName = "Vault"),
	ETAT_Mantle UMETA(DisplayName = "Mantle")
};

USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TEnumAsByte<ETraversalActionType> ActionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	bool bHasFrontLedge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector FrontLedgeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector FrontLedgeNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	bool bHasBackLedge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector BackLedgeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector BackLedgeNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	bool bHasBackFloor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector BackFloorLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float ObstacleHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float ObstacleDepth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	float BackLedgeHeight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<UPrimitiveComponent> HitComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Traversal")
	TObjectPtr<UAnimMontage> ChosenMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Traversal")
	float StartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Traversal")
	float EndTime;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType )
class AYS_API UTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraversalComponent();

	UFUNCTION(BlueprintNativeEvent)
	void TryTraversalAction(const FTraversalCheckInputs& Inputs, bool& TraversalCheckFailed, bool& MontageSelectionFailed);

	void AddCandidateSpline(ATraversalSpline* Spline);
	void RemoveCandidateSpline(ATraversalSpline* Spline);

	const TArray<TWeakObjectPtr<ATraversalSpline>>& GetCandidateSplines() const { return CandidateSplines; }
protected:
	virtual void BeginPlay() override;

	bool CacheValues();
	bool CalculateBestSpline(const FVector& HitLocation, const FVector& ActorLocation, const FVector& Forward);

	void GetLedgeTransforms(const FVector& HitLocation, const FVector& ActorLocation);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FCharacterPropertiesForTraversal CharacterProperties;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	FTraversalCheckResult CheckResult;

	UPROPERTY()
	TArray<TWeakObjectPtr<ATraversalSpline>> CandidateSplines;

	UPROPERTY()
	TObjectPtr<ATraversalSpline> ChosenSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float WDistToHit = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float WDistToActor = 0.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float WFacing = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float MinFacingDot = 0.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float MinSplineWidth = 60.0f;


public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};

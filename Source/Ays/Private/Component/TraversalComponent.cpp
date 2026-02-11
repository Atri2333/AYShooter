// Copyright Ayy3


#include "Component/TraversalComponent.h"

#include "Character/AysPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UTraversalComponent::UTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTraversalComponent::TryTraversalAction_Implementation(const FTraversalCheckInputs& Inputs, bool& TraversalCheckFailed, bool& MontageSelectionFailed)
{
	if (!CacheValues()) return;
	ChosenSpline = nullptr;
	CheckResult.bHasFrontLedge = false;
	CheckResult.bHasBackLedge = false;

	FVector ActorLocation = GetOwner()->GetActorLocation();
	float CapsuleRadius = CharacterProperties.CapsuleComponent->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = CharacterProperties.CapsuleComponent->GetScaledCapsuleHalfHeight();

	// First Trace: Forward
	FVector Start = ActorLocation + Inputs.TraceOriginOffset;
	FVector End = Start + Inputs.TraceForwardDirection * Inputs.TraceForwardDistance + Inputs.TraceEndOffset;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult Hit;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		Start,
		End,
		Inputs.TraceRadius,
		Inputs.TraceHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true
	);
	if (!Hit.bBlockingHit)
	{
		TraversalCheckFailed = true;
		return;
	}
	CheckResult.HitComponent = Hit.GetComponent();

	CalculateBestSpline(Hit.Location, ActorLocation, Inputs.TraceForwardDirection);
	if (!ChosenSpline) 
	{
		TraversalCheckFailed = true;
		return;
	}

	// 获取ledge位置和法线
	GetLedgeTransforms(Hit.Location, ActorLocation);

	if (!CheckResult.bHasFrontLedge)
	{
		TraversalCheckFailed = true;
		return;
	}

	// Second Trace: From the actors location up to the front ledge location to determine if there is room for the actor to move up to it
	const FVector HasRoomCheckFrontLedgeLocation =
		CheckResult.FrontLedgeLocation + CheckResult.FrontLedgeNormal * (CapsuleRadius + 2.f)
		+ FVector(0.f, 0.f, CapsuleHalfHeight + 2.f);

	FHitResult RoomCheckHit;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		ActorLocation,
		HasRoomCheckFrontLedgeLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		RoomCheckHit,
		true
	);

	// if (RoomCheckHit.bBlockingHit)
	// {
	// 	CheckResult.bHasFrontLedge = false;
	// 	TraversalCheckFailed = true;
	// 	return;
	// }

	CheckResult.ObstacleHeight =
		FMath::Abs(
			(ActorLocation
			- FVector(0.f, 0.f, CapsuleHalfHeight)
			- CheckResult.FrontLedgeLocation).Z);

	// Third Trace: Trace across the top of the obstacle from the front ledge to the back ledge
	// to see if there's room for the actor to move across it.
	const FVector HasRoomCheckBackLedgeLocation =
		CheckResult.BackLedgeLocation + CheckResult.BackLedgeNormal * (CapsuleRadius + 2.f)
		+ FVector(0.f, 0.f, CapsuleHalfHeight + 2.f);

	FHitResult TopSweepResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		HasRoomCheckFrontLedgeLocation,
		HasRoomCheckBackLedgeLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		TopSweepResult,
		true
	);
	if (TopSweepResult.bBlockingHit)
	{
		CheckResult.ObstacleDepth =
			(TopSweepResult.ImpactPoint - CheckResult.FrontLedgeLocation).Size2D();
		CheckResult.bHasBackLedge = false;
	}
	else
	{
		CheckResult.ObstacleDepth =
			(CheckResult.BackLedgeLocation - CheckResult.FrontLedgeLocation).Size2D();
		CheckResult.bHasBackLedge = true;

		// Fourth Trace: Trace downward from the back ledge location (using the height of the obstacle for the distance) to find the floor.
		// If there is a floor, save its location and the back ledges height (using the distance between the back ledge and the floor).
		// If no floor was found, invalidate the back floor.
		FHitResult DownTraceHit;
		UKismetSystemLibrary::CapsuleTraceSingle(
			this,
			HasRoomCheckBackLedgeLocation,
			CheckResult.BackLedgeLocation +
			(CheckResult.BackLedgeNormal * (CapsuleRadius + 2.f)) -
			FVector(0.f, 0.f, 50.f),
			CapsuleRadius,
			CapsuleHalfHeight,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			DownTraceHit,
			true
		);

		if (DownTraceHit.bBlockingHit)
		{
			CheckResult.bHasBackFloor = true;
			CheckResult.BackFloorLocation = DownTraceHit.ImpactPoint;
			CheckResult.BackLedgeHeight =
				FMath::Abs((CheckResult.BackLedgeLocation - CheckResult.BackFloorLocation).Z);
		}
		else
		{
			CheckResult.bHasBackFloor = false;
		}
	}
}

void UTraversalComponent::AddCandidateSpline(ATraversalSpline* Spline)
{
	if (!Spline) return;
	
	CandidateSplines.RemoveAll([](const TWeakObjectPtr<ATraversalSpline>& Ptr)
	{
		return !Ptr.IsValid();
	});
	
	CandidateSplines.AddUnique(Spline);
}

void UTraversalComponent::RemoveCandidateSpline(ATraversalSpline* Spline)
{
	if (!Spline) return;
	
	CandidateSplines.RemoveAll([Spline](const TWeakObjectPtr<ATraversalSpline>& Ptr)
	{
		return !Ptr.IsValid() || Ptr.Get() == Spline;
	});
}

void UTraversalComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UTraversalComponent::CacheValues()
{
	AAysPlayer* AysPlayer = Cast<AAysPlayer>(GetOwner());
	if (AysPlayer == nullptr) return false;

	CharacterProperties.CapsuleComponent = AysPlayer->GetCapsuleComponent();
	CharacterProperties.FppMesh = AysPlayer->GetFppMesh();
	CharacterProperties.TppMesh = AysPlayer->GetMesh();
	CharacterProperties.Speed = AysPlayer->GetVelocity().Size2D();

	return true;
}

bool UTraversalComponent::CalculateBestSpline(const FVector& HitLocation, const FVector& ActorLocation,
	const FVector& Forward)
{
	CandidateSplines.RemoveAll([](const TWeakObjectPtr<ATraversalSpline>& Ptr)
	{
		return !Ptr.IsValid();
	});

	ChosenSpline = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (const TWeakObjectPtr<ATraversalSpline>& Ptr : CandidateSplines)
	{
		ATraversalSpline* SplineActor = Ptr.Get();
		if (!SplineActor) continue;

		USplineComponent* SplineComp = SplineActor->FindComponentByClass<USplineComponent>();
		if (!SplineComp) continue;

		// Make sure the spline is wide enough
		const float SplineWidth = SplineComp->GetSplineLength();
		if (SplineWidth < MinSplineWidth) continue;

		const FVector Closest = SplineComp->FindLocationClosestToWorldLocation(
			HitLocation, ESplineCoordinateSpace::World);

		const float DistToHit = FVector::Dist(Closest, HitLocation);
		const float DistToActor = FVector::Dist(Closest, ActorLocation);

		const FVector DirToSpline = (Closest - ActorLocation).GetSafeNormal();
		const float FacingDot = FVector::DotProduct(Forward, DirToSpline);
		if (FacingDot < MinFacingDot) continue;

		const float Score = WDistToHit * DistToHit + WDistToActor * DistToActor - WFacing * FacingDot;
		if (Score < BestScore)
		{
			BestScore = Score;
			ChosenSpline = SplineActor;
		}
	}

	return ChosenSpline != nullptr;
}

void UTraversalComponent::GetLedgeTransforms(const FVector& HitLocation, const FVector& ActorLocation)
{
	if (ChosenSpline == nullptr) return;

	USplineComponent* SplineComp = ChosenSpline->FindComponentByClass<USplineComponent>();
	if (!SplineComp) return;

	// 防止在样条线的极端位置Traverse
	const FVector Closest = SplineComp->FindLocationClosestToWorldLocation(HitLocation, ESplineCoordinateSpace::Local);
	float DistanceAlongSpline = SplineComp->GetDistanceAlongSplineAtLocation(Closest, ESplineCoordinateSpace::Local);
	DistanceAlongSpline = FMath::Clamp(DistanceAlongSpline, MinSplineWidth / 2.f, SplineComp->GetSplineLength() - MinSplineWidth / 2.f);
	const FTransform TraversalTransform = SplineComp->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	CheckResult.bHasFrontLedge = true;
	CheckResult.FrontLedgeLocation = TraversalTransform.GetLocation();
	CheckResult.FrontLedgeNormal = TraversalTransform.GetRotation().GetUpVector();

	// 对面的ledge
	ATraversalSpline* OppositeTraversal = ChosenSpline->GetOppositeTraversal();
	if (!IsValid(OppositeTraversal))
	{
		CheckResult.bHasBackLedge = false;
		return;
	}
	USplineComponent* OppositeSplineComp = OppositeTraversal->FindComponentByClass<USplineComponent>();
	if (!IsValid(OppositeSplineComp))
	{
		CheckResult.bHasBackLedge = false;
		return;
	}

	const FTransform OppositeTraversalTransform = OppositeSplineComp->FindTransformClosestToWorldLocation(CheckResult.FrontLedgeLocation, ESplineCoordinateSpace::World);

	CheckResult.bHasBackLedge = true;
	CheckResult.BackLedgeLocation = OppositeTraversalTransform.GetLocation();
	CheckResult.BackLedgeNormal = OppositeTraversalTransform.GetRotation().GetUpVector();
}


void UTraversalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}


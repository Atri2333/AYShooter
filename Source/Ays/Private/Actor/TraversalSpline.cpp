// Copyright Ayy3


#include "Actor/TraversalSpline.h"

#include "Component/TraversalComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"


ATraversalSpline::ATraversalSpline()
{
	PrimaryActorTick.bCanEverTick = false;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	SetRootComponent(Spline);
	
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("TriggerVolume");
	TriggerVolume->SetupAttachment(GetRootComponent());
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void ATraversalSpline::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATraversalSpline::OnTriggerBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ATraversalSpline::OnTriggerEnd);
	}
}

void ATraversalSpline::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;

	if(UTraversalComponent* TraversalComp = OtherActor->FindComponentByClass<UTraversalComponent>())
	{
		TraversalComp->AddCandidateSpline(this);
	}
}

void ATraversalSpline::OnTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor)) return;

	if (UTraversalComponent* TraversalComp = OtherActor->FindComponentByClass<UTraversalComponent>())
	{
		TraversalComp->RemoveCandidateSpline(this);
	}
}

void ATraversalSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


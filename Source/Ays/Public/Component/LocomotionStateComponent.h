// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LocomotionStateComponent.generated.h"

struct FGameplayTag;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLocomotionStateChanged, const FGameplayTag& Tag, bool bAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AYS_API ULocomotionStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULocomotionStateComponent();

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer CurrentStates;
	
	FOnLocomotionStateChanged OnLocomotionStateChanged;
	
	void TryAddState(const FGameplayTag& Tag);

	void RemoveState(const FGameplayTag& Tag);

	bool HasState(const FGameplayTag& Tag) const;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};

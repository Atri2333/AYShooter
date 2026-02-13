#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponUIData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoInClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalAmmo;
};
// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AysAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API UAysAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:

	static UAssetManager* Get();
	
private:
	virtual void StartInitialLoading() override;	
	
};

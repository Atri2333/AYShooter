// Copyright Ayy3


#include "AysAssetManager.h"

#include "AysGameplayTags.h"


UAssetManager* UAysAssetManager::Get()
{
	check(GEngine);

	UAysAssetManager* AssetManager = Cast<UAysAssetManager>(GEngine->AssetManager);
	return AssetManager;
}

void UAysAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAysGameplayTags::InitializeNativeGameplayTags();
}

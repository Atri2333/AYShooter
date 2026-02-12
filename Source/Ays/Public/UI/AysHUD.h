// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AysHUD.generated.h"

struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
class AAysPlayer;
class AAysPlayerState;
class AAysPlayerController;
class UAysUserWidget;
/**
 * 
 */
UCLASS()
class AYS_API AAysHUD : public AHUD
{
	GENERATED_BODY()
public:

	void InitOverlay(AAysPlayerController* PC, AAysPlayerState* PS, AAysPlayer* AP, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& Params);
	
protected:

private:

	UPROPERTY()
	TObjectPtr<UAysUserWidget>  OverlayWidget;	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAysUserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	
};

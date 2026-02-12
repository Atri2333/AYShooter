// Copyright Ayy3


#include "UI/AysHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AysUserWidget.h"
#include "UI/WidgetController/AysWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"


void AAysHUD::InitOverlay(AAysPlayerController* PC, AAysPlayerState* PS, AAysPlayer* AP, UAbilitySystemComponent* ASC,
                          UAttributeSet* AS)
{
	OverlayWidget = CreateWidget<UAysUserWidget>(GetWorld(), OverlayWidgetClass);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, AP, ASC, AS);
	OverlayWidgetController = GetOverlayWidgetController(WidgetControllerParams);
	OverlayWidget->SetWidgetController(OverlayWidgetController);
	OverlayWidgetController->BroadcastInitialValues();
	OverlayWidget->AddToViewport();
}

UOverlayWidgetController* AAysHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

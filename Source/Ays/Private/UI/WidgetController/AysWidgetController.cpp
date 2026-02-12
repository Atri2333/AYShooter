// Copyright Ayy3


#include "UI/WidgetController/AysWidgetController.h"


void UAysWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& Params)
{
	AysPlayerController = Params.AysPlayerController;
	AysPlayerState = Params.AysPlayerState;
	AysPlayer = Params.AysPlayer;
	AbilitySystemComponent = Params.AbilitySystemComponent;
	AttributeSet = Params.AttributeSet;
}

void UAysWidgetController::BroadcastInitialValues()
{
}

void UAysWidgetController::BindCallbacksToDependencies()
{
	
}

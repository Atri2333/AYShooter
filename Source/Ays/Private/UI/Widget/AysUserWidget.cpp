// Copyright Ayy3


#include "UI/Widget/AysUserWidget.h"


void UAysUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

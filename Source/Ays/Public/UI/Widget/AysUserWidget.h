// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AysUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AYS_API UAysUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
	
	
};

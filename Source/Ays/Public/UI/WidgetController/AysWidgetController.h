// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AysWidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class AAysPlayer;
class AAysPlayerState;
class AAysPlayerController;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() = default;
	FWidgetControllerParams(AAysPlayerController* InPC, AAysPlayerState* InPS, AAysPlayer* InPlayer, UAbilitySystemComponent* InASC, UAttributeSet* InAttributeSet)
		: AysPlayerController(InPC), AysPlayerState(InPS), AysPlayer(InPlayer), AbilitySystemComponent(InASC), AttributeSet(InAttributeSet)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AAysPlayerController> AysPlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AAysPlayerState> AysPlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AAysPlayer> AysPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponUIDataChangedDelegate, const FWeaponUIData&, WeaponUIData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedDelegate, float, NewValue);

/**
 * 
 */
UCLASS()
class AYS_API UAysWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FWeaponUIDataChangedDelegate OnWeaponUIDataChanged;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChangedDelegate OnMaxHealthChanged;
	
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& Params);
	
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAysPlayerController> AysPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAysPlayerState> AysPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAysPlayer> AysPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};

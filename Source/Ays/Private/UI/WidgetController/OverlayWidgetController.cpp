// Copyright Ayy3


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AysAttributeSet.h"
#include "Character/AysPlayer.h"
#include "Component/WeaponComponent.h"


class UWeaponComponent;

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	const UAysAttributeSet* AysAttributeSet = CastChecked<UAysAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AysAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AysAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	// 绑定武器相关Delegate
	if (!IsValid(AysPlayer)) return;
	UWeaponComponent* WeaponComp = AysPlayer->GetWeaponComponent();
	if (IsValid(WeaponComp))
	{
		WeaponComp->OnWeaponUIDataChanged.AddLambda([this](const FWeaponUIData& WeaponUIData)
		{
			OnWeaponUIDataChanged.Broadcast(WeaponUIData);
		});
	}

	const UAysAttributeSet* AysAttributeSet = CastChecked<UAysAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AysAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			const float NewHealth = Data.NewValue;
			OnHealthChanged.Broadcast(NewHealth);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AysAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			const float NewMaxHealth = Data.NewValue;
			OnMaxHealthChanged.Broadcast(NewMaxHealth);
		});
}

// Copyright Ayy3


#include "AbilitySystem/AsyncTask/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"


UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCooldownChange,
			&UWaitCooldownChange::CooldownTagChanged);
	
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		WaitCooldownChange,
		&UWaitCooldownChange::OnActiveEffectApplied);	
	
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (IsValid(ASC))
		ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	// 只处理冷却结束
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectApplied(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 在这里处理冷却开始，因为要获取时间
	FGameplayTagContainer AssetTags;
	EffectSpec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectSpec.GetAllGrantedTags(GrantedTags);
	
	if (AssetTags.HasTag(CooldownTag) || GrantedTags.HasTag(CooldownTag))
	{
		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		const TArray<float> TimeRemainingArray = ASC->GetActiveEffectsTimeRemaining(Query);
		
		if (TimeRemainingArray.Num() > 0)
		{
			CooldownStart.Broadcast(TimeRemainingArray[0]);
		}
	}
}

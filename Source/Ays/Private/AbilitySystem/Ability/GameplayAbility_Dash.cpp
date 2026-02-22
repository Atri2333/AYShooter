// Copyright Ayy3


#include "AbilitySystem/Ability/GameplayAbility_Dash.h"

#include "AysGameplayTags.h"
#include "Character/AysPlayer.h"
#include "Component/FPSCharacterMovementComponent.h"
#include "Component/LocomotionStateComponent.h"
#include "Player/AysPlayerController.h"


FGameplayTagContainer* UGameplayAbility_Dash::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;

}

void UGameplayAbility_Dash::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FAysGameplayTags::Get().Cooldown, CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

}

void UGameplayAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Set Transient Pointers
	AysPlayer = CastChecked<AAysPlayer>(ActorInfo->AvatarActor.Get());
	FPSCMC = CastChecked<UFPSCharacterMovementComponent>(AysPlayer->GetCharacterMovement());
	AysPlayerController = CastChecked<AAysPlayerController>(AysPlayer->GetController());
	if (ULocomotionStateComponent * LocomotionStateComponent = AysPlayerController->LocomotionStateComponent)
	{
		for (const FGameplayTag& Tag: InvalidateLocomotionTags)
		{
			LocomotionStateComponent->RemoveState(Tag);
		}
	}
	
	FPSCMC->PerformDash();
}

void UGameplayAbility_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	
}

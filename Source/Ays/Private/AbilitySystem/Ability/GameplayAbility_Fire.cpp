// Copyright Ayy3


#include "AbilitySystem/Ability/GameplayAbility_Fire.h"

#include "Component/LocomotionStateComponent.h"
#include "Component/WeaponComponent.h"
#include "Curves/CurveVector.h"
#include "Data/WeaponDataAsset.h"
#include "Player/AysPlayerController.h"


UGameplayAbility_Fire::UGameplayAbility_Fire()
{
}

void UGameplayAbility_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (OwnerWeaponComp->GetCurrentWeapon() == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	OwnerPC = Cast<AAysPlayerController>(OwnerPlayer->GetController());
	ShotsFired = 0;
	
	FppFireMontage = WeaponDataAsset->GetFppFireMontageByTag(OwnerWeaponComp->GetCurrentWeaponTag());
	FppAimedFireMontage = WeaponDataAsset->GetFppAimedFireMontageByTag(OwnerWeaponComp->GetCurrentWeaponTag());
	bIsAutoFiring = OwnerWeaponComp->GetCurrentWeapon()->CanRepeatAttack();
	if (bIsAutoFiring)
	{
		FireInterval = 1.f / OwnerWeaponComp->GetCurrentWeapon()->GetFireRate();
		StartAutoFire();
	}
	else
	{
		DoFireOnce();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	
}

void UGameplayAbility_Fire::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearAutoFire();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_Fire::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UGameplayAbility_Fire::StartAutoFire()
{
	DoFireOnce();
	if (UWorld* World = GetWorld())
	{
		if (FireInterval > 0.f)
		{
			World->GetTimerManager().SetTimer(AutoFireTimerHandle,
			 this,
			 &ThisClass::DoFireOnce,
			 FireInterval,
			 true);
		}
	}
}

void UGameplayAbility_Fire::DoFireOnce_Implementation()
{
	++ShotsFired;
	
	ApplyRecoilOnce();
}

void UGameplayAbility_Fire::ClearAutoFire()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoFireTimerHandle);
	}
}

void UGameplayAbility_Fire::ApplyRecoilOnce()
{
	if (!OwnerPlayer->IsLocallyControlled() || !IsValid(OwnerPC)) return;

	const FGameplayTag& WeaponTag = OwnerWeaponComp->GetCurrentWeaponTag();
	const FWeaponData WeaponData = WeaponDataAsset->GetWeaponDataByTag(WeaponTag);
	if (!WeaponData.RecoilPerShotCurve) return;

	const float ShotIndex = FMath::Max(0, ShotsFired - 1);
	const FVector Recoil = WeaponData.RecoilPerShotCurve->GetVectorValue(ShotIndex);
	OwnerPC->SetRecoilInput(Recoil.X, Recoil.Y);
}

void UGameplayAbility_Fire::AddBlockLocomotionTags()
{
	AAysPlayerController* PC = Cast<AAysPlayerController>(OwnerPlayer->GetController());
	if (!IsValid(PC) || PC->LocomotionStateComponent == nullptr) return;
	for (const FGameplayTag& Tag: BlockLocomotionTags)
	{
		PC->LocomotionStateComponent->TryAddState(Tag);
	}
}

void UGameplayAbility_Fire::RemoveBlockLocomotionTags()
{
	AAysPlayerController* PC = Cast<AAysPlayerController>(OwnerPlayer->GetController());
	if (!IsValid(PC) || PC->LocomotionStateComponent == nullptr) return;
	for (const FGameplayTag& Tag: BlockLocomotionTags)
	{
		PC->LocomotionStateComponent->RemoveState(Tag);
	}
}

// Copyright Ayy3


#include "AbilitySystem/Ability/GameplayAbility_Fire.h"

#include "AbilitySystemComponent.h"
#include "Component/LocomotionStateComponent.h"
#include "Component/WeaponComponent.h"
#include "Curves/CurveVector.h"
#include "Data/WeaponDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/AysPlayerController.h"


UGameplayAbility_Fire::UGameplayAbility_Fire()
{
}

void UGameplayAbility_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (OwnerWeaponComp->GetCurrentWeapon() == nullptr || !CanFire())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (HasAuthority(&ActivationInfo))
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
				.AddUObject(this, &ThisClass::OnTargetDataReceived);
		}
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
		return;
	}
	
	
}

void UGameplayAbility_Fire::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearAutoFire();
	if (HasAuthority(&ActivationInfo))
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
				.RemoveAll(this);
		}
	}
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

// 蓝图里播放了AM和Cue
void UGameplayAbility_Fire::DoFireOnce_Implementation()
{
	// TODO: 客户端根据本地的弹药量判断是否EndAbility，但事实上应该根据服务器的弹药量来判断，可能会有些不同步，后续可以优化
	if (!CanFire())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	// 服务器权威校验
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (!CanFire())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}
	
	if (IsLocallyControlled())
	{
		const FVector MuzzleWorld = OwnerWeaponComp->GetCurrentWeaponSocketTransform(MuzzleSocketName).GetLocation();
		const FVector OwnerWorld = OwnerPlayer->GetActorLocation();
		const FVector RelativeOffset = MuzzleWorld - OwnerWorld;
		
		// 相机方向
		const FVector CamForward = OwnerPC->PlayerCameraManager->GetCameraRotation().Vector();
		const float MaxSpreadAngle = OwnerWeaponComp->CalculateMaxWeaponSpreadAngle();
		//const FVector OffsetDir = FMath::VRandCone(CamForward, FMath::DegreesToRadians(MaxSpreadAngle));
		const FVector OffsetDir = UWeaponComponent::VRandCone(CamForward, FMath::DegreesToRadians(MaxSpreadAngle));
		const FVector CamLoc = OwnerPC->PlayerCameraManager->GetCameraLocation();
		const FVector CamTraceEnd = CamLoc + OffsetDir * OwnerWeaponComp->GetCurrentWeaponFireDistance();
		
		// 第一次Trace，从相机位置向前Trace
		FHitResult CameraTraceHit;
		DoFireTrace(CamLoc, CamTraceEnd, CameraTraceHit);
		
		FVector FinalHitLocation = CameraTraceHit.bBlockingHit ? CameraTraceHit.ImpactPoint : CamTraceEnd;
		
		FVector MuzzleTraceDirection = (FinalHitLocation - MuzzleWorld).GetSafeNormal();
		
		// 设置要发送给服务器的TargetData
		FFireTargetData* FireTargetData = new FFireTargetData();
		FireTargetData->FireDirection = MuzzleTraceDirection;
		FireTargetData->RelativeMuzzleOffset = RelativeOffset;
		
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(FireTargetData);
		
		// 发送TargetData到服务器
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->ServerSetReplicatedTargetData(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				TargetDataHandle,
				FGameplayTag(),
				ASC->ScopedPredictionKey
			);
		}
		
		// 第二次Trace，预测表现
		FVector MuzzleTraceEnd = MuzzleWorld + MuzzleTraceDirection * OwnerWeaponComp->GetCurrentWeaponFireDistance();
		FHitResult MuzzleTraceHit;
		DoFireTrace(MuzzleWorld, MuzzleTraceEnd, MuzzleTraceHit);
		
		UKismetSystemLibrary::DrawDebugSphere(
			this,
			MuzzleTraceHit.ImpactPoint,
			15.f,
			12,
			FLinearColor::Green,
			2.f
		);
	}
	
	++ShotsFired;
	ApplyRecoilOnce();
	OwnerWeaponComp->FireWeapon();
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

bool UGameplayAbility_Fire::CanFire()
{
	return OwnerWeaponComp->CanFireWeapon();
}

void UGameplayAbility_Fire::DoFireTrace(const FVector& Start, const FVector& End, FHitResult& OutHitResult)
{
	UKismetSystemLibrary::CapsuleTraceSingle(
			this,
			Start,
			End,
			OwnerWeaponComp->GetCurrentWeaponFireWidth(),
			OwnerWeaponComp->GetCurrentWeaponFireWidth(),
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>({OwnerPlayer}),
			EDrawDebugTrace::None,
			OutHitResult,
			true
		);
}

// Server-only
void UGameplayAbility_Fire::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (!HasAuthority(&ActivationInfo)) return;
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ConsumeClientReplicatedTargetData(GetCurrentAbilitySpecHandle(),
			ActivationInfo.GetActivationPredictionKey());
	}
	
	if (DataHandle.Data.Num() ==0 || DataHandle.Data[0] == nullptr)
	{
		return;
	}
	
	const FGameplayAbilityTargetData* BaseData = DataHandle.Data[0].Get();
	const FFireTargetData* FireData = static_cast<const FFireTargetData*>(BaseData);
	if (!FireData)
	{
		return;
	}
	
	const FVector FireDir = FireData->FireDirection;
	const FVector RelativeMuzzleOffset = FireData->RelativeMuzzleOffset;
	
	FHitResult MuzzleTraceHit;
	DoFireTrace(
		OwnerPlayer->GetActorLocation() + RelativeMuzzleOffset, 
		OwnerPlayer->GetActorLocation() + RelativeMuzzleOffset + FireDir * OwnerWeaponComp->GetCurrentWeaponFireDistance(), 
		MuzzleTraceHit);
	
	UKismetSystemLibrary::DrawDebugSphere(
			this,
			MuzzleTraceHit.ImpactPoint,
			20.f,
			12,
			FLinearColor::Red,
			2.f
		);
}

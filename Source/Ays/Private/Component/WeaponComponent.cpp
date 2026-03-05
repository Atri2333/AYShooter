// Copyright Ayy3


#include "Component/WeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AysGameplayTags.h"
#include "WeaponUIData.h"
#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "Data/WeaponDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Ability/GameplayAbility_WeaponBase.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UWeaponComponent, Inventory);
}

void UWeaponComponent::InitWeaponComponent()
{
	// 初始化Cached指针
	OwnerASC = CastChecked<UAysAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	OwnerPlayer = CastChecked<AAysPlayer>(GetOwner());
	
	if (WeaponDataAsset)
	{
		// Grant初始武器Ability
		if (OwnerPlayer->HasAuthority())
		{
			for (const auto& StartupWeaponAbility : WeaponDataAsset->StartupWeaponAbilities)
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupWeaponAbility);
				FGameplayTag AbilityTag = AbilitySpec.Ability->GetAssetTags().First();
				// 绑定输入ID
				if (WeaponDataAsset->AbilityToInputIDMap.Contains(AbilityTag))
				{
					const EAysAbilityInputID InputID = WeaponDataAsset->AbilityToInputIDMap[AbilityTag];
					AbilitySpec.InputID = static_cast<int32>(InputID);
				}
				OwnerASC->GiveAbility(AbilitySpec);
			}
		}		
	}
}

// 在Grant Equip Ability后调用, both server and client
void UWeaponComponent::EquipInitialWeapon()
{
	FWeaponData InitialWeaponData = WeaponDataAsset->GetWeaponDataByTag(DefaultWeaponTag);
	// 在DataAsset找到了初始武器数据
	if (InitialWeaponData.WeaponTag == DefaultWeaponTag)
	{
		OwnedWeaponTags.AddTag(DefaultWeaponTag);
		if (OwnerPlayer->HasAuthority())
		{
			AWeapon* InitialWeapon = GetWorld()->SpawnActor<AWeapon>(InitialWeaponData.WeaponClass);
			if (InitialWeapon)
			{
				Inventory.Add(InitialWeapon);
				EquipWeapon(InitialWeapon);
			}
		}
	}
}

void UWeaponComponent::EquipWeapon(AWeapon* InWeapon)
{
	if (!IsValid(InWeapon)) return;

	CurrentWeapon = InWeapon;
	CurrentWeapon->OnWeaponFiredDelegate.Clear();
	CurrentWeapon->OnWeaponFiredDelegate.AddUObject(this, &UWeaponComponent::OnWeaponStatChanged);
	

	// 激活装备Ability
	const FGameplayTag& EquipTag = FAysGameplayTags::Get().Ability_Weapon_Equip;
	OwnerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(EquipTag));
	
	// 设置位置
	// TODO: 玩家角色挂到Fpp，其余挂到Tpp
	InWeapon->AttachToComponent(OwnerPlayer->GetFppGunSceneComp(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	InWeapon->OnEquipped();
		// OnWeaponStatChanged();
}

void UWeaponComponent::SwitchWeapon(const FGameplayTag& NewWeaponTag)
{
	
}

void UWeaponComponent::FireWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->FireLogic();
}

bool UWeaponComponent::CanFireWeapon() const
{
	if (!IsValid(CurrentWeapon)) return false;
	return CurrentWeapon->CanFire();
}

bool UWeaponComponent::CanReloadWeapon() const
{
	if (!IsValid(CurrentWeapon)) return false;
	return CurrentWeapon->CanReload();
}

void UWeaponComponent::ApplyReloadLogic()
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->ApplyReloadLogic();
}

void UWeaponComponent::OnWeaponStatChanged()
{
	// 广播UI数据变化
	const FWeaponUIData WeaponUIData = GetWeaponUIData();
	OnWeaponUIDataChanged.Broadcast(WeaponUIData);
}

void UWeaponComponent::OnWeaponStatChanged(const FWeaponUIData& WeaponUIData)
{
	OnWeaponUIDataChanged.Broadcast(WeaponUIData);
}

const FWeaponUIData UWeaponComponent::GetWeaponUIData() const
{
	if (!IsValid(CurrentWeapon)) return FWeaponUIData();
	const FWeaponUIData WeaponUIData = CurrentWeapon->GetUIData();
	return WeaponUIData;
}


FGameplayTag UWeaponComponent::GetCurrentWeaponTag() const
{
	if (IsValid(CurrentWeapon))
	{
		return CurrentWeapon->WeaponTag;
	}
	return FGameplayTag();
}

FTransform UWeaponComponent::GetCurrentWeaponSocketTransform(const FName& SocketName) const
{
	if (!IsValid(CurrentWeapon)) return FTransform::Identity;
	
	const FTransform SocketTransform = CurrentWeapon->GetSocketTransform(SocketName);
	return SocketTransform;
}

float UWeaponComponent::GetCurrentWeaponFireDistance() const
{
	if (!IsValid(CurrentWeapon)) return 0.f;
	return CurrentWeapon->GetFireDistance();
}

float UWeaponComponent::GetCurrentWeaponFireWidth() const
{
	if (!IsValid(CurrentWeapon)) return 0.f;
	return CurrentWeapon->GetAttackWidth();
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}



void UWeaponComponent::OnRep_CurrentWeapon()
{
	EquipWeapon(CurrentWeapon);
	OnWeaponStatChanged();
}
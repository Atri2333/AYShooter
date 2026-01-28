// Copyright Ayy3


#include "Component/WeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AysAbilitySystemComponent.h"
#include "Data/WeaponDataAsset.h"
#include "Net/UnrealNetwork.h"


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
		FWeaponData InitialWeaponData = WeaponDataAsset->GetWeaponDataByTag(DefaultWeaponTag);
		// 在DataAsset找到了初始武器数据
		if (InitialWeaponData.WeaponTag == DefaultWeaponTag)
		{
			OwnedWeaponTags.AddTag(DefaultWeaponTag);
			AWeapon* InitialWeapon = GetWorld()->SpawnActor<AWeapon>(InitialWeaponData.WeaponClass);
			if (InitialWeapon)
			{
				CurrentWeapon = InitialWeapon;
				Inventory.Add(InitialWeapon);
				EquipWeapon(CurrentWeapon);
			}
		}
	}
}

void UWeaponComponent::EquipWeapon(AWeapon* InWeapon)
{
	if (!IsValid(InWeapon)) return;
	
	//TODO: 设置Equip的Gameplay Ability，用来播放动画，设置HUD等
	
	// 设置位置
	InWeapon->AttachToComponent(OwnerPlayer->GetFppGunSceneComp(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	InWeapon->OnEquipped();
}

void UWeaponComponent::SwitchWeapon(const FGameplayTag& NewWeaponTag)
{
	
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
}

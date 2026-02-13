// Copyright Ayy3


#include "Weapon/GunWeapon.h"

#include "WeaponUIData.h"
#include "Net/UnrealNetwork.h"


AGunWeapon::AGunWeapon()
{
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(WeaponMesh, MagazineSocketName);
}

void AGunWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AGunWeapon, AmmoInMag, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGunWeapon, TotalAmmo, COND_None, REPNOTIFY_Always);
}

const FWeaponUIData AGunWeapon::GetUIData() const
{
	FWeaponUIData Data;
	Data.WeaponTag = WeaponTag;
	Data.AmmoInClip = AmmoInMag;
	Data.TotalAmmo = TotalAmmo;
	return Data;
}

void AGunWeapon::FireLogic()
{
	Super::FireLogic();

	--AmmoInMag;
	OnWeaponFiredDelegate.Broadcast(GetUIData());
}

bool AGunWeapon::CanFire() const
{
	return AmmoInMag > 0;
}

bool AGunWeapon::CanReload() const
{
	return AmmoInMag < MagazineCapacity && TotalAmmo > 0;
}

void AGunWeapon::ApplyReloadLogic()
{
	const int32 AmmoNeeded = MagazineCapacity - AmmoInMag;
	const int32 AmmoToReload = FMath::Min(AmmoNeeded, TotalAmmo);
	AmmoInMag += AmmoToReload;
	TotalAmmo -= AmmoToReload;
	OnWeaponFiredDelegate.Broadcast(GetUIData());
}

void AGunWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

void AGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGunWeapon::OnRep_AmmoInMag(const int32 OldAmmoInMag)
{
	OnWeaponFiredDelegate.Broadcast(GetUIData());
}

void AGunWeapon::OnRep_TotalAmmo(const int32 OldTotalAmmo)
{
	OnWeaponFiredDelegate.Broadcast(GetUIData());
}

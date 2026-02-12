// Copyright Ayy3


#include "Weapon/GunWeapon.h"


AGunWeapon::AGunWeapon()
{
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(WeaponMesh, MagazineSocketName);
}

const FWeaponUIData AGunWeapon::GetUIData() const
{
	FWeaponUIData Data;
	Data.WeaponTag = WeaponTag;
	Data.AmmoInClip = AmmoInMag;
	Data.TotalAmmo = TotalAmmo;
	return Data;
}

void AGunWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

void AGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

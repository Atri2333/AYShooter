// Copyright Ayy3


#include "Weapon/GunWeapon.h"


AGunWeapon::AGunWeapon()
{
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(WeaponMesh, MagazineSocketName);
}

void AGunWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

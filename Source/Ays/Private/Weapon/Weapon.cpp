// Copyright Ayy3


#include "Weapon/Weapon.h"

#include "WeaponUIData.h"


AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void AWeapon::OnEquipped()
{
	OnWeaponFiredDelegate.Broadcast(GetUIData());
}

FTransform AWeapon::GetSocketTransform(FName SocketName) const
{
	const FTransform SocketTransform = WeaponMesh->GetSocketTransform(SocketName);
	return SocketTransform;
}

const FWeaponUIData AWeapon::GetUIData() const
{
	return FWeaponUIData();
}

void AWeapon::FireLogic()
{
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


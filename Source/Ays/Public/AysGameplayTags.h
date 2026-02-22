// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FAysGameplayTags
{
	static const FAysGameplayTags& Get() {return Instance;}
	// 在AysAssetManager中进行初始化
	static void InitializeNativeGameplayTags();

	// 状态 Tags (持续性的)
	FGameplayTag State_Locomotion;
	FGameplayTag State_Locomotion_Idle;
	FGameplayTag State_Locomotion_Walk;
	FGameplayTag State_Locomotion_Sprint;
	FGameplayTag State_Locomotion_Crouch;
	FGameplayTag State_Locomotion_Slide;
	FGameplayTag State_Locomotion_LeanLeft;
	FGameplayTag State_Locomotion_LeanRight;

	FGameplayTag State_Combat;
	FGameplayTag State_Combat_Aiming;
	FGameplayTag State_Combat_Firing;

	FGameplayTag State_Locomotion_BlockSprint;
	

	// 动作 Tags (瞬时的)
	FGameplayTag Action_Jump;

	// Weapon Tags
	FGameplayTag Weapon;
	FGameplayTag Weapon_Gun;
	FGameplayTag Weapon_Gun_AR;

	// Ability Tags
	FGameplayTag Ability_Weapon;
	FGameplayTag Ability_Weapon_Equip;
	FGameplayTag Ability_Weapon_Unequip;
	FGameplayTag Ability_Weapon_Reload;
	FGameplayTag Ability_Weapon_Fire;
	FGameplayTag Ability_Dash;
	
	// Gameplay Event Tags
	FGameplayTag Event_Weapon_ReloadConfirm;
	FGameplayTag Event_Weapon_ReloadFinished;
	
	// Cooldown
	FGameplayTag Cooldown;
	FGameplayTag Cooldown_Dash;
	
private:
	static FAysGameplayTags Instance;	
};
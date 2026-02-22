// Copyright Ayy3

#include "AysGameplayTags.h"

#include "GameplayTagsManager.h"

FAysGameplayTags FAysGameplayTags::Instance;

void FAysGameplayTags::InitializeNativeGameplayTags()
{
	// State Locomotion
	Instance.State_Locomotion = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion"),
		FString("Locomotion")
	);
	Instance.State_Locomotion_Crouch = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.Crouch"),
		FString("Crouch State")
	);
	Instance.State_Locomotion_Idle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.Idle"),
		FString("Idle State")
	);
	Instance.State_Locomotion_Slide = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.Slide"),
		FString("Slide State")
	);
	Instance.State_Locomotion_Sprint = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.Sprint"),
		FString("Sprint State")
	);
	Instance.State_Locomotion_Walk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.Walk"),
		FString("Walk State")
	);
	Instance.State_Locomotion_LeanLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.LeanLeft"),
		FString("Lean Left State")
	);
	Instance.State_Locomotion_LeanRight = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.LeanRight"),
		FString("Lean Right State")
	);
	

	Instance.State_Combat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Combat"),
		FString("Combat")
	);
	Instance.State_Combat_Aiming = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Combat.Aiming"),
		FString("Aiming State")
	);
	Instance.State_Combat_Firing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Combat.Firing"),
		FString("Firing State")
	);

	Instance.State_Locomotion_BlockSprint = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Locomotion.BlockSprint"),
		FString("Block Sprint State")
	);

	// Action
	Instance.Action_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Action.Jump"),
		FString("Jump Action")
	);

	// Weapon
	Instance.Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Weapon"),
		FString("Weapon Tag")
	);
	Instance.Weapon_Gun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Weapon.Gun"),
		FString("Gun Weapon Tag")
	);
	Instance.Weapon_Gun_AR = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Weapon.Gun.AR"),
		FString("Assault Rifle Gun Weapon Tag")
	);

	// Ability
	Instance.Ability_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Weapon"),
		FString("Weapon Ability Tag")
	);
	Instance.Ability_Weapon_Equip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Weapon.Equip"),
		FString("Weapon Equip Ability Tag")
	);
	Instance.Ability_Weapon_Unequip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Weapon.Unequip"),
		FString("Weapon Unequip Ability Tag")
	);
	Instance.Ability_Weapon_Reload = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Weapon.Reload"),
		FString("Weapon Reload Ability Tag")
	);
	Instance.Ability_Weapon_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Weapon.Fire"),
		FString("Weapon Fire Ability Tag")
	);
	Instance.Ability_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Dash"),
		FString("Dash Ability Tag")
	);
	
	// Gameplay Event
	Instance.Event_Weapon_ReloadConfirm = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Weapon.ReloadConfirm"),
		FString("Weapon Reload Confirm Event Tag")
	);
	Instance.Event_Weapon_ReloadFinished = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Weapon.ReloadFinished"),
		FString("Weapon Reload Finished Event Tag, used in AM Notify")
	);
	
	// Cooldown
	Instance.Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown"),
		FString("Cooldown Tag")
	);
	Instance.Cooldown_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Dash"),
		FString("Dash Cooldown Tag")
	);
}

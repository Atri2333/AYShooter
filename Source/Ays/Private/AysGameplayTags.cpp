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

	// Action
	Instance.Action_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Action.Jump"),
		FString("Jump Action")
	);
}

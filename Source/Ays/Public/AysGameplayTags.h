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

	// 动作 Tags (瞬时的)
	FGameplayTag Action_Jump;
	
private:
	static FAysGameplayTags Instance;	
};
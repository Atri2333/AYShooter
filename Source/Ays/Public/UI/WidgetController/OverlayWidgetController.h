// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AysWidgetController.h"
#include "OverlayWidgetController.generated.h"

/*
 * 考虑我们需要对哪些属性做监听
 * 1. 在ASC上记录的GAS相关属性，例如Health，体力等
 * 2. 在PlayerState上记录的属性，例如击杀数，死亡数等
 * 3. 其他我们想要在UI上展示的属性，例如当前武器，弹药量，这里直接通过角色类的WeaponComp获取（可能有更好的方法）
 * 绑定过程在BindCallbacksToDependencies里实现
 */

class UWeaponDataAsset;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AYS_API UOverlayWidgetController : public UAysWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;
	
};

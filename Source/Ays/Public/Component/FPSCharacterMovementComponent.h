// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "Character/AysPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovementComponent.generated.h"

class AAysPlayer;
class ULocomotionStateComponent;
struct FGameplayTag;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_MAX			UMETA(Hidden),
};
/**
 * 
 */
UCLASS()
class AYS_API UFPSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_FPS : public FSavedMove_Character
	{
		// Flags
		uint8 Saved_bWantsToSprint:1;
		// other variables
		uint8 Saved_bWantsToSlide:1;
	public:
		FSavedMove_FPS();

		// 检测当前移动和新移动是否可以合并
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		// 重置SavedMove
		virtual void Clear() override;
		// 将本次移动的关键布尔状态压缩到位标志位
		virtual uint8 GetCompressedFlags() const override;
		// 将本次移动的状态保存到SavedMove中
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		// 和SetMoveFor相反，将SavedMove中的状态应用到Character上，主要在重放和纠正状态应用
		virtual void PrepMoveFor(ACharacter* Character) override;
	};

	class FNetworkPredictionData_Client_FPS : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_FPS(const UCharacterMovementComponent& ClientMovement);

		// 分配我们自定义的FSavedMove_FPS
		virtual FSavedMovePtr AllocateNewMove() override;
	};

	// Movement Safe Variable代表可以在Perform Move和Phys*里正常使用的变量
	// 这种变量绝对不能是角色属性的引用
	// 否则在Replay的时候会出问题，因为Replay前后角色属性可能不一样
	// 一般这种变量要同步到SavedMove里去，这样Server才能收到正确的状态
	bool Safe_bWantsToSprint = false;
	bool Safe_bWantsToSlide = false;

	UPROPERTY(Transient)
	TObjectPtr<AAysPlayer> AysPlayer;

protected:

	virtual void InitializeComponent() override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	bool IsCustomMovementMode(ECustomMovementMode Mode) const;
	
	
public:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	// 获取我们自己的NetworkPredictionData_Client
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanAttemptJump() const override;
	virtual bool CanCrouchInCurrentState() const override;
	
	void InitLocomotionComponent();
	void InitBasicLocomotion();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InterpCrouchAlpha(float DeltaTime);
	void HandleStateChange(const FGameplayTag& Tag, bool bAdded);

	UPROPERTY(EditAnywhere)
	float Sprint_MaxWalkSpeed = 750.f;

	UPROPERTY(EditAnywhere)
	float Walk_MaxWalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly) float Slide_MinSpeed=400;
	UPROPERTY(EditDefaultsOnly) float Slide_EnterImpulse=400;
	UPROPERTY(EditDefaultsOnly) float Slide_GravityForce=200;
	UPROPERTY(EditDefaultsOnly) float Slide_Friction=.1;
	
	UPROPERTY(EditDefaultsOnly) float DashImpulse = 600.f;

	UPROPERTY(EditAnywhere, Category = "Deprecared")
	float CrouchAlpha = 0.f;

	UPROPERTY(EditAnywhere, Category = "Deprecared")
	float CrouchTransitionSpeed = 10.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULocomotionStateComponent> LocomotionStateComponent;

	// Slide
private:
	void EnterSlide();
	void ExitSlide(bool bFall = false);
	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	
	bool SlidedDuringThisCrouch;
	
	// Dash
public:
	void PerformDash();
};

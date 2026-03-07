// Copyright Ayy3


#include "Component/FPSCharacterMovementComponent.h"

#include "AysGameplayTags.h"
#include "Character/AysPlayer.h"
#include "Component/LocomotionStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Player/AysPlayerController.h"


UFPSCharacterMovementComponent::FSavedMove_FPS::FSavedMove_FPS()
{
	Saved_bWantsToSprint = false;
}

void UFPSCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AysPlayer = CastChecked<AAysPlayer>(GetOwner());
	
}

bool UFPSCharacterMovementComponent::FSavedMove_FPS::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                    ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_FPS* NewFPSMove = static_cast<const FSavedMove_FPS*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewFPSMove->Saved_bWantsToSprint)
	{
		// Sprint状态不同，不能合并
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = false;
}

uint8 UFPSCharacterMovementComponent::FSavedMove_FPS::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bWantsToSprint)
	{
		// Custom flag 0 represents sprinting
		Result |= FLAG_Custom_0;
	}
	return Result;
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::SetMoveFor(ACharacter* Character, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	if (IsValid(Character))
	{
		if (UFPSCharacterMovementComponent* FPSCMC = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			Saved_bWantsToSprint = FPSCMC->Safe_bWantsToSprint;
		}
	}
}

void UFPSCharacterMovementComponent::FSavedMove_FPS::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	if (IsValid(Character))
	{
		if (UFPSCharacterMovementComponent* FPSCMC = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			FPSCMC->Safe_bWantsToSprint = Saved_bWantsToSprint;
		}
	}
}

UFPSCharacterMovementComponent::FNetworkPredictionData_Client_FPS::FNetworkPredictionData_Client_FPS(
	const UCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr UFPSCharacterMovementComponent::FNetworkPredictionData_Client_FPS::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_FPS());
}

void UFPSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_FPS::FLAG_Custom_0) != 0;
}

void UFPSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
}

FNetworkPredictionData_Client* UFPSCharacterMovementComponent::GetPredictionData_Client() const
{
	// UE内部也用了const_cast
	if (ClientPredictionData == nullptr)
	{
		UFPSCharacterMovementComponent* MutableThis = const_cast<UFPSCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_FPS(*this);
	}

	return ClientPredictionData;
}

bool UFPSCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(ECustomMovementMode::CMOVE_Slide);
}

bool UFPSCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

bool UFPSCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump();
}

void UFPSCharacterMovementComponent::InitLocomotionComponent()
{
	AAysPlayer* Player = Cast<AAysPlayer>(CharacterOwner);
	if (IsValid(Player))
	{
		// LocomotionStateComp在PC上，因此需要PC有效才能Retrieve出LocomotionStateComp
		// 因此需要在PC有效的时候调用本函数
		AAysPlayerController* PC = Cast<AAysPlayerController>(Player->GetController());
		if (IsValid(PC))
		{
			LocomotionStateComponent = PC->LocomotionStateComponent;
		}
	}
}

void UFPSCharacterMovementComponent::InitBasicLocomotion()
{
	MaxWalkSpeed = Walk_MaxWalkSpeed;
	if (LocomotionStateComponent != nullptr)
		LocomotionStateComponent->OnLocomotionStateChanged.AddUObject(this, &ThisClass::HandleStateChange);
}

void UFPSCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFPSCharacterMovementComponent::InterpCrouchAlpha(float DeltaTime)
{
	const float TargetAlpha = IsCrouching() ? 1.f : 0.f;

	if (!FMath::IsNearlyEqual(CrouchAlpha, TargetAlpha))
	{
		CrouchAlpha = FMath::FInterpTo(CrouchAlpha, TargetAlpha, DeltaTime, CrouchTransitionSpeed);
	}
}

void UFPSCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	InterpCrouchAlpha(DeltaTime);
}

void UFPSCharacterMovementComponent::HandleStateChange(const FGameplayTag& Tag, bool bAdded)
{
	const FAysGameplayTags& Tags = FAysGameplayTags::Get();
	if (Tag == Tags.Action_Jump && bAdded)
	{
		if (IsValid(CharacterOwner))
		{
			AAysPlayer* Player = Cast<AAysPlayer>(CharacterOwner);
			if (IsValid(Player))
			{
				if (!Player->TryTraversal())
				{
					CharacterOwner->Jump();
				}
			}
			
		}
	}
	else if (Tag == Tags.State_Locomotion_Sprint)
	{
		if (bAdded)
		{
			// 会通过SetMoveFor同步到SavedMove里传给Server
			Safe_bWantsToSprint = true;
		}
		else
		{
			Safe_bWantsToSprint = false;
		}
	}
	else if (Tag == Tags.State_Locomotion_Crouch)
	{
		if (bAdded)
		{
			if (IsValid(CharacterOwner))
			{
				CharacterOwner->Crouch();
			}
		}
		else
		{
			if (IsValid(CharacterOwner))
			{
				CharacterOwner->UnCrouch();
			}
		}
	}
}

void UFPSCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	
	// 按下下蹲键，判断是否可以Slide
	if (MovementMode == MOVE_Walking && bWantsToCrouch && !SlidedDuringThisCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.Length() > Slide_MinSpeed && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	// 取消下蹲且当前在Slide状态则恢复Walking状态
	if (!bWantsToCrouch && IsCustomMovementMode(ECustomMovementMode::CMOVE_Slide))
	{
		ExitSlide();
	}

	if (!bWantsToCrouch)
	{
		// 重置Slide
		SlidedDuringThisCrouch = false;
	}
}

void UFPSCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

bool UFPSCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode Mode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == Mode;
}

void UFPSCharacterMovementComponent::EnterSlide()
{
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, ECustomMovementMode::CMOVE_Slide);
}

void UFPSCharacterMovementComponent::ExitSlide(bool bFall)
{	
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	if (!bFall)
	{
		// 在地上刹车停止的话就不重复Slide了，在空中的话则可以重复Slide
		SlidedDuringThisCrouch = true;
		SetMovementMode(MOVE_Walking);
	}
	else
	{
		SetMovementMode(MOVE_Falling);
	}
}

void UFPSCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	// 从PhysWalking借鉴过来的
	
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	bool bFall = !GetSlideSurface(SurfaceHit);
    if (bFall || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
    {
    	ExitSlide(bFall);
    	StartNewPhysics(deltaTime, Iterations);
    	return;
    }
	
	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;
	
	// Perform the move, substep
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->GetController() || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
		const FFindFloorResult OldFloor = CurrentFloor;
		
		// Surface Gravity
		// v = v0 + at
		Velocity += Slide_GravityForce * FVector::DownVector * timeTick;
		
		// Strafe
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
		
		// Calc Velocity
		if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(timeTick, Slide_Friction, false, GetMaxBrakingDeceleration());
		}
		
		ApplyRootMotionToVelocity(timeTick);
		
		FHitResult Hit(1.f);
		FVector Adjusted = Velocity * timeTick;
		SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit);
		
		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, timeTick, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
		
		FHitResult NewSurfaceHit;
		bFall = !GetSlideSurface(NewSurfaceHit);
		if (bFall || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
		{
			ExitSlide(bFall);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
	
		// Update Outgoing Velocity & Acceleration
		if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
		}
	}
}

bool UFPSCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, AysPlayer->GetIgnoreCharacterParams());
}

void UFPSCharacterMovementComponent::PerformDash()
{
	const FVector DashDir = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
	
	Velocity = DashImpulse * (DashDir + FVector::UpVector * .2f);
	
	
	SetMovementMode(MOVE_Falling);
}

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

float UFPSCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking && Safe_bWantsToSprint && !IsCrouching())
		return Sprint_MaxWalkSpeed;
	
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxSpeed();
	
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_MaxSpeed;
	case CMOVE_WallRun:
		return MaxWallRunSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
		return -1.f;
	}
}

float UFPSCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom) return Super::GetMaxBrakingDeceleration();
	
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return BrakingDecelerationSliding;
	case CMOVE_WallRun:
		return 0.f;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
		return -1.f;
	}
}

void UFPSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                       const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	
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
	return Super::CanAttemptJump() || IsWallRunning();
}

bool UFPSCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	// Tip: 跳之前预先记录
	const bool bWasWallRunning = IsWallRunning();
	if (Super::DoJump(bReplayingMoves))
	{
		// 增加切向速度
		if (bWasWallRunning)
		{
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FVector Start = OldLocation;
			const FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
			const FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
			const auto Params = AysPlayer->GetIgnoreCharacterParams();
	
			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpOffForce;
			
		}
		return true;
	}
	return false;
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
			CharacterOwner->Jump();
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
	
	// Falling是进入WallRun的条件
	if (IsFalling())
	{
		TryWallRun();
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
	case CMOVE_WallRun:
		PhysWallRun(deltaTime, Iterations);
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
	
	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
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
		// 只计算水平分量，在MoveAlongFloor里会适配斜坡
		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * Slide_GravityForce * deltaTime;
		
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
		// SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit);
		//
		// UE_LOG(LogTemp, Warning, TEXT("Hit Time = %f"), Hit.Time);
		// if (Hit.Time < 1.f)
		// {
		// 	HandleImpact(Hit, timeTick, Adjusted);
		// 	SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		// }
		
		FStepDownResult StepDownResult;
		
		const bool bZeroDelta = Adjusted.IsNearlyZero();
		
		if (!bZeroDelta)
		{
			MoveAlongFloor(Velocity, timeTick, &StepDownResult);
		}
		
		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
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

bool UFPSCharacterMovementComponent::TryWallRun()
{
	// 只能通过Falling切换到WallRun
	if (!IsFalling()) return false;
	// 水平速度需要够快
	if (Velocity.SizeSquared2D() < MinWallRunSpeed) return false;
	// 下落速度不能太大
	if (Velocity.Z < -MaxVerticalWallRunSpeed) return false;
	
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * CapR() * 2;
	const FVector RightEnd = Start + UpdatedComponent->GetRightVector() * CapR() * 2;
	auto Params = AysPlayer->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	
	// 高度不能太低
	if (GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallRunHeight), "BlockAll", Params))
	{
		return false;
	}
	
	// Left Cast
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, "BlockAll", Params);
	if (WallHit.IsValidBlockingHit() && (WallHit.Normal | Velocity) < 0)
	{
		Safe_bWallRunIsRight = false;
	}
	else
	{
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit() && (WallHit.Normal | Velocity) < 0)
		{
			Safe_bWallRunIsRight = true;
		}
		else
		{
			return false;
		}
	}
	
	const FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
	{
		return false;
	}
	// All Conditions Passed
	
	Velocity = ProjectedVelocity;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallRunSpeed);
	SetMovementMode(MOVE_Custom, CMOVE_WallRun);
	
	return true;
}

void UFPSCharacterMovementComponent::PhysWallRun(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->GetController() && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
	bJustTeleported = false;
	float remainingTime = deltaTime;
	
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->GetController() || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FVector Start = OldLocation;
		const FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
		const FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
		auto Params = AysPlayer->GetIgnoreCharacterParams();
		const float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
		
		// 加速度方向向外，准备退出
		if (!WallHit.IsValidBlockingHit() ||bWantsToPullAway || Acceleration.IsNearlyZero())
		{
			EndWallRun();
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		
		// Clamp Acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f;
		
		// Apply Acceleration
		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		
		const float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();
		const bool bVelUp = Velocity.Z > 0.f;
		Velocity.Z += GetGravityZ() * WallRunGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : TangentAccel) * timeTick;
		if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2) || Velocity.Z < -MaxVerticalWallRunSpeed)
		{
			EndWallRun();
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		
		// Compute move parameters
		const FVector Delta = timeTick * Velocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		if ( bZeroDelta )
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			const FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);

		}
		
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
		
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
	}
	
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Start = OldLocation;
	const FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
	const FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
	auto Params = AysPlayer->GetIgnoreCharacterParams();
	
	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallRunHeight * .5f), "BlockAll", Params);

	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
	{
		EndWallRun();
	}

}

void UFPSCharacterMovementComponent::EndWallRun()
{
	SetMovementMode(MOVE_Falling);
}

bool UFPSCharacterMovementComponent::IsServer() const
{
	return CharacterOwner->HasAuthority();
}

float UFPSCharacterMovementComponent::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UFPSCharacterMovementComponent::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

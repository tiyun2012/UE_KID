#include "TiPawnMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UTiPawnMovementComponent::UTiPawnMovementComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    MovementMode = ETiMovementMode::MOVE_None;
    PreviousMovementMode = ETiMovementMode::MOVE_None;

    WalkableFloorZ = FMath::Cos(FMath::DegreesToRadians(MaxWalkableSlopeAngle));

    bIsCrouching = false;
    bWantsToCrouch = false;
    bPressedJump = false;
    bIsSprinting = false;

    JumpCurrentCount = 0;
    JumpMaxCount = 1;
    JumpKeyHoldTime = 0.0f;
    JumpForceTimeRemaining = 0.0f;
    CachedGravityZ = 0.0f;
}

void UTiPawnMovementComponent::SetMovementMode(ETiMovementMode NewMode)
{
    if (MovementMode == NewMode) return;

    PreviousMovementMode = MovementMode;
    MovementMode = NewMode;

    if (MovementMode == ETiMovementMode::MOVE_Walking)
    {
        Velocity.Z = 0.0f;
        JumpCurrentCount = 0;
    }
    else if (MovementMode == ETiMovementMode::MOVE_Falling)
    {
        OnFalling();
    }
}
void UTiPawnMovementComponent::PerformGroundCheck()
{
    if (!PawnOwner || !UpdatedComponent || !GetWorld()) return;

    UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(UpdatedComponent);
    if (!Capsule) return;

    float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
    float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
    FVector CapsuleLocation = Capsule->GetComponentLocation();

    // 1. Calculate Trace Points
    FVector TraceStart = CapsuleLocation;
    FVector TraceEnd = TraceStart;

    // Determine check distance (Feet + Buffer)
    float CheckDistance = GroundCheckDistance + 5.0f;
    TraceEnd.Z -= (CapsuleHalfHeight + CheckDistance);

    // --- DEBUG SETTINGS ---
#if WITH_EDITOR
    bool bShowDebug = true;          // Set to false to disable
    float DebugLifeTime = 0.010f;      // Lines stay for 5 seconds (visible after unpausing)
    float DebugThickness = 2.0f;
#endif
    // ----------------------

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(PawnOwner);
    QueryParams.bTraceComplex = false;

    FHitResult Hit;

    // 2. Perform Line Trace
    bool bHitGround = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams
    );

    // 3. Fallback: Sphere Sweep (if line trace fails)
    if (!bHitGround)
    {
        FVector SweepStart = TraceStart;
        SweepStart.Z -= CapsuleRadius; // Adjust start to fit sphere

        bHitGround = GetWorld()->SweepSingleByChannel(
            Hit, SweepStart, TraceEnd, FQuat::Identity, ECC_WorldStatic,
            FCollisionShape::MakeSphere(CapsuleRadius * 0.8f), QueryParams
        );
    }

    // --- VISUALIZE TRACE PROGRESS ---
#if WITH_EDITOR
    if (bShowDebug)
    {
        // Draw Start Point (Green) - The "Center"
        DrawDebugSphere(GetWorld(), TraceStart, 5.0f, 12, FColor::Green, false, DebugLifeTime, 0, DebugThickness);

        // Draw End Point (Red) - The "Limit"
        DrawDebugSphere(GetWorld(), TraceEnd, 5.0f, 12, FColor::Red, false, DebugLifeTime, 0, DebugThickness);

        // Draw The Ray (Cyan)
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Cyan, false, DebugLifeTime, 0, DebugThickness);

        if (bHitGround)
        {
            // Draw Impact Point (Yellow Star)
            DrawDebugPoint(GetWorld(), Hit.Location, 10.0f, FColor::Yellow, false, DebugLifeTime);

            // Log Values to Output Window
            UE_LOG(LogTemp, Warning, TEXT("[GroundCheck] HIT! FloorZ: %.2f | Dist: %.2f"), Hit.Location.Z, Hit.Distance);
        }
        else
        {
            // Log Miss
            UE_LOG(LogTemp, Log, TEXT("[GroundCheck] MISS - Air"));
        }
    }
#endif
    // ---------------------------------

    // 4. Logic State Switching
    if (bHitGround && IsWalkable(Hit))
    {
        if (MovementMode == ETiMovementMode::MOVE_Falling || MovementMode == ETiMovementMode::MOVE_None)
        {
            OnLanded(Hit);
            SetMovementMode(ETiMovementMode::MOVE_Walking);

            // Snap to floor
            FVector NewLocation = CapsuleLocation;
            NewLocation.Z = Hit.Location.Z + CapsuleHalfHeight + 0.1f;
            Capsule->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        }
        else if (MovementMode == ETiMovementMode::MOVE_Walking)
        {
            // Sticky feet logic
            float DistToFloor = FMath::Abs((CapsuleLocation.Z - CapsuleHalfHeight) - Hit.Location.Z);
            if (DistToFloor > 0.5f && DistToFloor < CheckDistance && Velocity.Z <= 0.0f)
            {
                FVector NewLocation = CapsuleLocation;
                NewLocation.Z = Hit.Location.Z + CapsuleHalfHeight + 0.1f;
                Capsule->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
            }
        }
    }
    else
    {
        if (MovementMode == ETiMovementMode::MOVE_Walking)
        {
            SetMovementMode(ETiMovementMode::MOVE_Falling);
        }
    }
}

void UTiPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;

    if (FMath::IsNearlyZero(CachedGravityZ)) CachedGravityZ = GetGravityZ();

    // Initial Spawn Check
    if (MovementMode == ETiMovementMode::MOVE_None)
    {
        PerformGroundCheck();
        if (MovementMode == ETiMovementMode::MOVE_None) SetMovementMode(ETiMovementMode::MOVE_Falling);
    }

    FVector InputVector = ConsumeInputVector().GetClampedToMaxSize(1.0f);

    if (bPressedJump)
    {
        Jump();
        bPressedJump = false;
    }

    float CurrentMaxSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;

    // Movement Logic
    if (MovementMode == ETiMovementMode::MOVE_Walking)
    {
        FVector TargetVelocity = InputVector * CurrentMaxSpeed;
        TargetVelocity.Z = 0.0f;

        float CurrentAccel = InputVector.IsNearlyZero() ? Deceleration : Acceleration;
        FVector NewVel = FMath::VInterpConstantTo(FVector(Velocity.X, Velocity.Y, 0.f), TargetVelocity, DeltaTime, CurrentAccel);

        // Friction
        if (InputVector.IsNearlyZero())
        {
            NewVel *= FMath::Max(1.0f - GroundFriction * DeltaTime, 0.0f);
            if (NewVel.SizeSquared() < 100.0f) NewVel = FVector::ZeroVector;
        }

        Velocity.X = NewVel.X;
        Velocity.Y = NewVel.Y;
        Velocity.Z = 0.0f;

        PerformGroundCheck();
    }
    else if (MovementMode == ETiMovementMode::MOVE_Falling)
    {
        FVector TargetVelocity = InputVector * CurrentMaxSpeed;
        FVector HorizontalVel = FVector(Velocity.X, Velocity.Y, 0.f);

        // Air Control
        HorizontalVel = FMath::VInterpTo(HorizontalVel, TargetVelocity, DeltaTime, AirControl);

        Velocity.X = HorizontalVel.X;
        Velocity.Y = HorizontalVel.Y;
        Velocity.Z += CachedGravityZ * GravityScale * DeltaTime;

        if (Velocity.Z < -MaxFallSpeed) Velocity.Z = -MaxFallSpeed;
    }

    // Apply Move
    FVector MoveDelta = Velocity * DeltaTime;
    if (!MoveDelta.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(MoveDelta, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(MoveDelta, 1.0f - Hit.Time, Hit.Normal, Hit);

            if (MovementMode == ETiMovementMode::MOVE_Falling && IsWalkable(Hit))
            {
                OnLanded(Hit);
                SetMovementMode(ETiMovementMode::MOVE_Walking);
                Velocity.Z = 0.0f;
            }
        }
    }

    UpdateComponentVelocity();
}

bool UTiPawnMovementComponent::IsWalkable(const FHitResult& Hit) const
{
    if (!Hit.IsValidBlockingHit()) return false;
    return Hit.Normal.Z >= WalkableFloorZ;
}

void UTiPawnMovementComponent::OnLanded(const FHitResult& Hit)
{
    JumpCurrentCount = 0;
}

void UTiPawnMovementComponent::OnFalling()
{
    // Falling logic
}

void UTiPawnMovementComponent::Jump()
{
    if (JumpCurrentCount < JumpMaxCount)
    {
        Velocity.Z = JumpZVelocity;
        SetMovementMode(ETiMovementMode::MOVE_Falling);
        JumpCurrentCount++;
    }
}

void UTiPawnMovementComponent::Crouch()
{
    // Crouch logic
}

void UTiPawnMovementComponent::Uncrouch()
{
    // Uncrouch logic
}
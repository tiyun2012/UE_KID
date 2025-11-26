#include "TiPawnMovementComponent.h"

void UTiPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    // 1. Get Input
    FVector InputVector = ConsumeInputVector().GetClampedToMaxSize(1.0f);

    // 2. Define "Proposed" Move
    FVector MoveDelta = InputVector * MovementSpeed * DeltaTime;

    // Start Location (for calculating real velocity later)
    FVector OldLocation = UpdatedComponent->GetComponentLocation();

    // 3. Move
    if (!MoveDelta.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(MoveDelta, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(MoveDelta, 1.0f - Hit.Time, Hit.Normal, Hit);
        }
    }

    // 4. Calculate Real Velocity
    // Formula: (NewPos - OldPos) / DeltaTime
    FVector NewLocation = UpdatedComponent->GetComponentLocation();
    Velocity = (NewLocation - OldLocation) / DeltaTime;

    // 5. Update the Component's internal state (Critical for GetVelocity() to work)
    UpdateComponentVelocity();
}
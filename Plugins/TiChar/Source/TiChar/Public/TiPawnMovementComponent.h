

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TiPawnMovementComponent.generated.h"

UCLASS()
class TICHAR_API UTiPawnMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Speed in cm/s */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementSpeed = 600.0f;

    // We do NOT need 'StoredInputVector' or 'RotationSpeed' here.
    // The Component handles physics. The Pawn handles Rotation logic.
};
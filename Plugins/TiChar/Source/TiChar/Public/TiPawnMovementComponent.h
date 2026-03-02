#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TiPawnMovementComponent.generated.h"

// Custom movement modes
UENUM(BlueprintType)
enum class ETiMovementMode : uint8
{
    MOVE_None        UMETA(DisplayName = "None"),
    MOVE_Walking     UMETA(DisplayName = "Walking"),
    MOVE_Falling     UMETA(DisplayName = "Falling"),
    MOVE_Swimming    UMETA(DisplayName = "Swimming"),
    MOVE_Flying      UMETA(DisplayName = "Flying"),
    MOVE_Custom      UMETA(DisplayName = "Custom")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TICHAR_API UTiPawnMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:
    UTiPawnMovementComponent(const FObjectInitializer& ObjectInitializer);

    //~ Begin UMovementComponent Interface
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UMovementComponent Interface

    // --- Movement Logic ---
    void PerformGroundCheck();

    UFUNCTION(BlueprintCallable, Category = "TiMovement")
    ETiMovementMode GetTiMovementMode() const { return MovementMode; }

    UFUNCTION(BlueprintCallable, Category = "TiMovement")
    void SetMovementMode(ETiMovementMode NewMode);

    // Actions
    void Jump();
    void Crouch();
    void Uncrouch();
    void SetJumpPressed(bool bNewJumpPressed) { bPressedJump = bNewJumpPressed; }

protected:
    bool IsWalkable(const FHitResult& Hit) const;
    void OnLanded(const FHitResult& Hit);
    void OnFalling();

public:
    // --- Config ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float MaxWalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float MaxSprintSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float Acceleration = 2048.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float Deceleration = 2048.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float JumpZVelocity = 420.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float JumpMaxHoldTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float GravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float MaxFallSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float GroundCheckDistance = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float MaxWalkableSlopeAngle = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float GroundFriction = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float AirControl = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiMovement")
    float AirFriction = 0.0f;

    // --- State ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    ETiMovementMode MovementMode;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    ETiMovementMode PreviousMovementMode;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    uint8 bIsCrouching : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    uint8 bWantsToCrouch : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    uint8 bPressedJump : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TiMovement|State")
    uint8 bIsSprinting : 1;

    // Velocity vector (inherited from UMovementComponent but accessed frequently)
    // FVector Velocity; // Already exists in parent

private:
    float WalkableFloorZ;
    float JumpCurrentCount;
    float JumpMaxCount;
    float JumpKeyHoldTime;
    float JumpForceTimeRemaining;
    float CachedGravityZ;
};
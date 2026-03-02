#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "TiPawnMovementComponent.h"
#include "TiPawnChar.generated.h"

// Forward declarations
class UCapsuleComponent;
class UTiPawnMovementComponent;
class UInputMappingContext;
class UInputAction;
class UTiInputConfig;
class USpringArmComponent;
class UCameraComponent; // <--- Changed from GameplayCamera

UCLASS()
class TICHAR_API ATiPawnChar : public APawn
{
    GENERATED_BODY()

public:
    ATiPawnChar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void BeginPlay() override;
    virtual void NotifyControllerChanged() override; // <--- The missing link fixed here

    // Input handler functions
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void JumpAction(const FInputActionValue& Value);
    void JumpActionReleased(const FInputActionValue& Value);

    // Rotation function
    void ApplyRotation(float DeltaTime);

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // --- CRITICAL OVERRIDE ---
    virtual UPawnMovementComponent* GetMovementComponent() const override { return TiPawnMovement; }

    // --- MOVEMENT STATE GETTERS ---
    UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
    ETiMovementMode GetCurrentMovementMode() const;

    UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
    bool IsMoving() const;

    UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
    bool IsFalling() const;

    UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
    bool IsWalking() const;

    UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
    UTiPawnMovementComponent* GetTiMovementComponent() const { return TiPawnMovement; }

    // --- ROTATION SETTINGS ---
    UPROPERTY(EditAnywhere, Category = "Ti|Movement|Rotation")
    float MovingRotationSpeed = 7.0f;

    UPROPERTY(EditAnywhere, Category = "Ti|Movement|Rotation")
    float IdleRotationSpeed = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Ti|Movement|Rotation")
    float MaxIdleCameraAngle = 90.0f;

    UPROPERTY(EditAnywhere, Category = "Ti|Movement|Rotation")
    float TurnStopTolerance = 2.0f;

    // Cache movement mode
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ti|Movement")
    ETiMovementMode CurrentMovementMode;

protected:
    // Components
    UPROPERTY(Category = "TiCharacter", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* TiPawnCapsule;

    UPROPERTY(Category = "TiCharacter", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* TiPawnMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTiPawnMovementComponent* TiPawnMovement;

    // --- STANDARD CAMERA SETUP ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* TiCameraSpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* TiFollowCamera; // <--- Standard Camera

    // Input config
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ti|Input")
    UTiInputConfig* TiInputConfig;

private:
    bool bIsTurningInPlace = false;
    void UpdateMovementModeCache();
};
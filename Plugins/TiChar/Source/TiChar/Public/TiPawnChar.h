// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "TiPawnChar.generated.h"
// Forward declarations (avoid heavy includes in headers)
class UCapsuleComponent;
class UTiPawnMovementComponent;
class UInputMappingContext;
class UInputAction;
class UTiInputConfig;
class UGameplayCameraComponent;
class UGameplayCameraRigComponent;
class USpringArmComponent;

// --- STEP 1: ADD THIS ENUM ---
UENUM(BlueprintType)
enum class ETiMovementState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Walking     UMETA(DisplayName = "Walking"),
	Falling     UMETA(DisplayName = "Falling"),
	Jumping     UMETA(DisplayName = "Jumping")
};

UCLASS()
class TICHAR_API ATiPawnChar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATiPawnChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the controller changes (possessed/unpossessed) - BEST PLACE FOR MAPPING CONTEXTS
	virtual void NotifyControllerChanged() override;

	// === Input handler functions ===
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);
	void ApplyRotation(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ti|Movement")
	ETiMovementState CurrentMovementState;
	UFUNCTION(BlueprintCallable, Category = "Ti|Movement")
	ETiMovementState GetMovementState() const;
	//protected component
protected:
	UPROPERTY(Category = "TiCharacter", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* TiPawnCapsule;

	UPROPERTY(Category = "TiCharacter", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TiPawnMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTiPawnMovementComponent* TiPawnMovement;

	// Camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayCameraComponent> TiPawnCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayCameraRigComponent> TiCameraRig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> TiCameraSpringArm;

	// --- Rotation Settings ---
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float RotationInterpSpeed = 10.0f;



	FRotator TargetRotation; // Internal variable to store where we want to face
	// === Enhanced Input setup ===

	// Input config
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ti|Input")
	UTiInputConfig* TiInputConfig;

	// --- Physics Rotation Settings ---

	// How "stiff" the spring is. Higher = Snappier. Lower = Looser/Heavier.
	UPROPERTY(EditAnywhere, Category = "Ti|Movement|Physics")
	float RotationStiffness = 20.0f;

	// How much friction. 
	// 1.0 = Oscillates (Wobbles). 
	// 2.0 = Critical Damping (Smooth stop, no wobble). Recommended ~2.0 to 5.0
	UPROPERTY(EditAnywhere, Category = "Ti|Movement|Physics")
	float RotationDamping = 3.0f;

	// Internal memory of how fast we are currently spinning (Degrees per second)
	float CurrentYawSpeed = 0.0f;
	// ADD THIS:
	UPROPERTY(Transient) // Transient means "don't save this to disk", just memory
	FFloatSpringState RotationSpringState;

	// --- ROTATION SETTINGS (Add these) ---

	// Speed when moving (usually fast, e.g., 10.0f)
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float WalkingRotationSpeed = 7.0f;

	// Speed when stationary and turning (usually medium, e.g., 6.0f)
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float TurnInPlaceSpeed = 4.5f;

	// Default speed for small adjustments (slow, e.g., 4.0f)
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float IdleRotationSpeed = 3.0f;

	// Angle gap required to start a turn-in-place (e.g., 90.0 degrees)
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float MaxIdleCameraAngle = 90.0f;

	// Angle gap where we STOP turning (e.g., 5.0 degrees)
	UPROPERTY(EditAnywhere, Category = "Ti|Movement")
	float TurnStopTolerance = 2.0f;

	// Internal tracking state
	bool bIsTurningInPlace = false;




};
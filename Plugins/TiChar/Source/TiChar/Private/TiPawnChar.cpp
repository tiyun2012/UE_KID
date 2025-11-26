// Fill out your copyright notice in the Description page of Project Settings.

#include "TiPawnChar.h"
#include "Kismet/KismetMathLibrary.h" // For getting camera rotation
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TiPawnMovementComponent.h"
//#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/GameplayCameraRigComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "TiInputConfig.h"
#include "Core/CameraAsset.h" 
// -----------------------
// Sets default values
ATiPawnChar::ATiPawnChar()
{
	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// 1. Create Capsule
	TiPawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TiPawnCapsule"));
	RootComponent = TiPawnCapsule;
	TiPawnCapsule->InitCapsuleSize(42.f, 90.0f);

	// 2. Create Mesh
	TiPawnMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TiPawnMesh"));
	TiPawnMesh->SetupAttachment(RootComponent);
	//TiPawnMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	TiPawnMesh->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -90.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);


	// Load default mesh if available
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TiMeshAsset(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	if (TiMeshAsset.Succeeded())
	{
		TiPawnMesh->SetSkeletalMesh(TiMeshAsset.Object);
	}

	// 3. Create Movement Component
	TiPawnMovement = CreateDefaultSubobject<UTiPawnMovementComponent>(TEXT("TiPawnMovement"));
	TiPawnMovement->UpdatedComponent = RootComponent;

	// 4. Create Camera Components
	TiCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	TiCameraSpringArm->SetupAttachment(RootComponent);
	TiCameraSpringArm->TargetArmLength = 300.0f;
	TiCameraSpringArm->bUsePawnControlRotation = true;

	TiCameraRig = CreateDefaultSubobject<UGameplayCameraRigComponent>(TEXT("CameraRig"));
	TiCameraRig->SetupAttachment(TiCameraSpringArm, USpringArmComponent::SocketName);

	TiPawnCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	TiPawnCamera->SetupAttachment(TiCameraRig);

	// --- FIX: Load the default Camera Asset ---


	// 5. Load Default Input Config (CORRECT PLACE)
	static ConstructorHelpers::FObjectFinder<UTiInputConfig> InputConfigAsset(TEXT("/Game/TIZone/IA_IMC_TiDataAsset.IA_IMC_TiDataAsset"));
	if (InputConfigAsset.Succeeded())
	{
		TiInputConfig = InputConfigAsset.Object;
	}
}

// Called when the game starts or when spawned
void ATiPawnChar::BeginPlay()
{
	Super::BeginPlay();
}

// This function runs whenever a Controller possesses this pawn (Spawn, Late Join, Respawn)
void ATiPawnChar::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Mapping Context here! 
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				// Safe to add here.
				if (TiInputConfig && TiInputConfig->DefaultMappingContext)
				{
					Subsystem->AddMappingContext(TiInputConfig->DefaultMappingContext, 0);
				}
			}
		}
	}
}

void ATiPawnChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Only BIND actions here. Mapping Context is handled in NotifyControllerChanged.
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (TiInputConfig)
		{
			if (TiInputConfig->MoveAction)
			{
				EIC->BindAction(TiInputConfig->MoveAction, ETriggerEvent::Triggered, this, &ATiPawnChar::Move);
			}

			if (TiInputConfig->LookAction)
			{
				EIC->BindAction(TiInputConfig->LookAction, ETriggerEvent::Triggered, this, &ATiPawnChar::Look);
			}

			if (TiInputConfig->JumpAction)
			{
				EIC->BindAction(TiInputConfig->JumpAction, ETriggerEvent::Started, this, &ATiPawnChar::JumpAction);
			}
		}
	}
}

// --- CHANGE 3: IMPLEMENT THE HELPER FUNCTION ---
ETiMovementState ATiPawnChar::GetMovementState() const
{
	// Safety Check: If we don't have a movement component, we are just Idle.
	if (!TiPawnMovement)
	{
		return ETiMovementState::Idle;
	}

	// 1. Check Physics (Are we in the air?)
	// Note: We will implement 'IsFalling()' in Phase 2, for now let's assume false to get it compiling
	// Or if you already pasted the MovementComponent code, this will work:
	if (TiPawnMovement->IsFalling())
	{
		// If moving up (+Z), we are jumping. If moving down (-Z), we are falling.
		return (GetVelocity().Z > 0.0f) ? ETiMovementState::Jumping : ETiMovementState::Falling;
	}

	// 2. Check Speed (Are we moving on ground?)
	// We check only X and Y (Horizontal) speed.
	if (GetVelocity().Size2D() > 10.0f)
	{
		return ETiMovementState::Walking;
	}

	// 3. Default
	return ETiMovementState::Idle;
}

void ATiPawnChar::Move(const FInputActionValue& Value)
{
	FVector2D MoveInput = Value.Get<FVector2D>();

	if (Controller && !MoveInput.IsZero())
	{
		// 1. Get Camera Direction (Control Rotation)
		const FRotator ControlRot = Controller->GetControlRotation();
		const FRotator YawRot(0, ControlRot.Yaw, 0);

		const FVector Fwd = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		FVector Direction = (Fwd * MoveInput.Y) + (Right * MoveInput.X);

		// 2. STANDARD UNREAL CALL
		// This automatically pushes 'Direction' into UPawnMovementComponent::ControlInputVector
		AddMovementInput(Direction);
	}
}


void ATiPawnChar::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ATiPawnChar::JumpAction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Jump triggered!"));
	// TODO: Call TiPawnMovement->PerformJump() here later
}

// TiPawnChar.cpp

// TiPawnChar.cpp

void ATiPawnChar::ApplyRotation(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	FVector HorizontalVel = FVector(Velocity.X, Velocity.Y, 0.0f);
	float Speed = HorizontalVel.Size();

	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = CurrentRot;

	float CurrentInterpSpeed = IdleRotationSpeed;

	// 1. MOVING: Face Direction
	if (Speed > 10.0f)
	{
		TargetRot = HorizontalVel.ToOrientationRotator();
		CurrentInterpSpeed = WalkingRotationSpeed;
		bIsTurningInPlace = false;
	}
	// 2. IDLE: Turn to Camera Logic
	else if (Controller)
	{
		FRotator CameraRot = Controller->GetControlRotation();
		float RelativeGap = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, CameraRot.Yaw));

		// Hysteresis Logic:
		// If already turning, keep going until gap is small (< TurnStopTolerance)
		// If NOT turning, only start if gap is big (> MaxIdleCameraAngle)
		if (bIsTurningInPlace)
		{
			if (RelativeGap < TurnStopTolerance) bIsTurningInPlace = false;
		}
		else
		{
			if (RelativeGap > MaxIdleCameraAngle) bIsTurningInPlace = true;
		}

		if (bIsTurningInPlace)
		{
			TargetRot = CameraRot;
			CurrentInterpSpeed = TurnInPlaceSpeed;
		}
	}

	// Apply Rotation
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, CurrentInterpSpeed);
	NewRot.Pitch = 0.0f;
	NewRot.Roll = 0.0f;
	SetActorRotation(NewRot);
}


void ATiPawnChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// --- CHANGE 4: UPDATE THE STATE EVERY FRAME ---
	CurrentMovementState = GetMovementState();
	// ----------------------------------------------

	// (Your existing rotation code goes here...)
	ApplyRotation(DeltaTime);
}
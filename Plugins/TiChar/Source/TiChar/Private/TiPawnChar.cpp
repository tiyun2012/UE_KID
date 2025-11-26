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

//// Called every frame
//void ATiPawnChar::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// 1. Get Velocity
//	FVector Velocity = GetVelocity();
//	float Speed = Velocity.Size(); // Get speed (length of vector)
//
//	// --- DEBUG START ---
//	// A. Print Text to Screen (Yellow text, top left)
//	if (GEngine)
//	{
//		FString DebugMsg = FString::Printf(TEXT("Velocity: %s | Speed: %.2f"), *Velocity.ToString(), Speed);
//		// Key: -1 (New line every frame), Time: 0.0f (1 frame only), Color: Yellow
//		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, DebugMsg);
//	}
//
//	// B. Draw Red Arrow in World (Only if moving)
//	if (Speed > 1.0f)
//	{
//		FVector Start = GetActorLocation();
//		FVector End = Start + Velocity; // Arrow points where you are going
//		DrawDebugDirectionalArrow(GetWorld(), Start, End, 50.0f, FColor::Red, false, -1.0f, 0, 5.0f);
//	}
//	// --- DEBUG END ---
//
//	// 2. Rotation Logic
//	if (Speed > 1.0f)
//	{
//		// MOVING: Face Velocity
//		TargetRotation = Velocity.ToOrientationRotator();
//	}
//	else
//	{
//		// IDLE: Face Camera (Turn-in-Place)
//		FRotator CurrentRot = GetActorRotation();
//		FRotator CameraRot = Controller ? Controller->GetControlRotation() : CurrentRot;
//
//		// Check if camera is > 90 degrees away from character
//		float YawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, CameraRot.Yaw);
//		if (FMath::Abs(YawDelta) > 90.0f)
//		{
//			TargetRotation = CameraRot;
//		}
//	}
//
//	// Apply Rotation (Common for both cases)
//	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);
//	NewRotation.Pitch = 0.0f;
//	NewRotation.Roll = 0.0f;
//	SetActorRotation(NewRotation);
//}

//void ATiPawnChar::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// 1. Get Velocity Data
//	FVector Velocity = GetVelocity();
//	float Speed = Velocity.Size();
//
//	// 2. Determine Logic
//	bool bShouldRotate = false;
//
//	if (Speed > 10.0f)
//	{
//		// Moving: Target is velocity direction
//		TargetRotation = Velocity.ToOrientationRotator();
//		bShouldRotate = true;
//	}
//	else
//	{
//		// Idle: Target is Camera (if angle is large)
//		FRotator CurrentRot = GetActorRotation();
//		FRotator CameraRot = Controller ? Controller->GetControlRotation() : CurrentRot;
//
//		// Check deadzone
//		float YawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, CameraRot.Yaw);
//		if (FMath::Abs(YawDelta) > MaxIdleCameraAngle)
//		{
//			TargetRotation = CameraRot;
//			bShouldRotate = true;
//		}
//	}
//
//	// 3. APPLY ACCELERATION PHYSICS
//	// We only run physics if we have a target, OR if we still have momentum (CurrentYawSpeed != 0)
//	if (bShouldRotate) // or your custom condition
//	{
//		float CurrentYaw = GetActorRotation().Yaw;
//		float TargetYaw = TargetRotation.Yaw;
//
//		// 1. Unwrap (Shortest Path)
//		float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
//		float VirtualTargetYaw = CurrentYaw + DeltaYaw;
//
//		// 2. PHYSICS INTERPOLATION
//		// Note: We pass 'RotationSpringState' instead of a speed variable.
//		// The struct remembers the velocity for us.
//		float NewYaw = UKismetMathLibrary::FloatSpringInterp(
//			CurrentYaw,
//			VirtualTargetYaw,
//			RotationSpringState,  // <--- Pass the struct here
//			RotationStiffness,
//			RotationDamping,
//			DeltaTime,
//			1.0f,                 // Mass (Standard is 1.0)
//			1.0f                  // Target Velocity amount (Standard is 1.0)
//		);
//
//		// 3. Apply
//		FRotator NewRot = GetActorRotation();
//		NewRot.Yaw = NewYaw;
//		SetActorRotation(NewRot);
//	}
//}
void ATiPawnChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. Get Data
	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size();
	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = CurrentRot;

	// 2. Determine Target
	// (We keep your logic: Face Velocity if moving, Face Camera if Idle+Turning)
	bool bIsMoving = Speed > 10.0f;

	if (bIsMoving)
	{
		TargetRot = Velocity.ToOrientationRotator();
	}
	else
	{
		FRotator CameraRot = Controller ? Controller->GetControlRotation() : CurrentRot;

		// Only turn if camera is far away (Standard Deadzone)
		if (FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, CameraRot.Yaw)) > MaxIdleCameraAngle)
		{
			TargetRot = CameraRot;
		}
	}

	// 3. SMOOTH ROTATION (The Industry Standard)
	// We use RInterpTo. It handles the "Shortest Path" automatically (no 360 spin bugs).

	// TWEAK THIS VALUE:
	// 6.0f = Smooth, heavy feel (The Division, Assassin's Creed)
	// 12.0f = Snappy, responsive (Fortnite, Valorant)
	float InterpolationSpeed = 6.0f;

	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, InterpolationSpeed);

	// 4. Clean up Pitch/Roll (Keep character upright)
	NewRot.Pitch = 0.0f;
	NewRot.Roll = 0.0f;

	SetActorRotation(NewRot);
}
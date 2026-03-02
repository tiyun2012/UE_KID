#include "TiPawnChar.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h" // <--- Standard Camera Header
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TiInputConfig.h"
#include "UObject/ConstructorHelpers.h"

ATiPawnChar::ATiPawnChar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. Capsule
    TiPawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TiPawnCapsule"));
    RootComponent = TiPawnCapsule;
    TiPawnCapsule->InitCapsuleSize(42.f, 90.0f);
    TiPawnCapsule->SetCollisionProfileName(TEXT("Pawn"));

    // Disable physics gravity so movement component handles it
    TiPawnCapsule->SetSimulatePhysics(false);
    TiPawnCapsule->SetEnableGravity(false);

    // 2. Mesh
    TiPawnMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TiPawnMesh"));
    TiPawnMesh->SetupAttachment(RootComponent);
    TiPawnMesh->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

    // 3. Movement
    TiPawnMovement = CreateDefaultSubobject<UTiPawnMovementComponent>(TEXT("TiPawnMovement"));
    TiPawnMovement->UpdatedComponent = RootComponent;

    // 4. Standard Camera Setup (Corrected)
    TiCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    TiCameraSpringArm->SetupAttachment(RootComponent);
    TiCameraSpringArm->TargetArmLength = 300.0f;
    TiCameraSpringArm->bUsePawnControlRotation = true;

    // Replaced 'TiCameraRig' logic with standard 'TiFollowCamera'
    TiFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    TiFollowCamera->SetupAttachment(TiCameraSpringArm, USpringArmComponent::SocketName);
    TiFollowCamera->bUsePawnControlRotation = false;
}

void ATiPawnChar::BeginPlay()
{
    Super::BeginPlay();

    //// Force Mesh Reset
    //if (TiPawnMesh)
    //{
    //    TiPawnMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    //    TiPawnMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    //}

    if (TiPawnMovement)
    {
        TiPawnMovement->PerformGroundCheck();
        UpdateMovementModeCache();
    }
}

void ATiPawnChar::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (TiInputConfig && TiInputConfig->DefaultMappingContext)
            {
                Subsystem->AddMappingContext(TiInputConfig->DefaultMappingContext, 0);
            }
        }
    }
}

void ATiPawnChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (!TiInputConfig) return;

        if (TiInputConfig->MoveAction)
            EIC->BindAction(TiInputConfig->MoveAction, ETriggerEvent::Triggered, this, &ATiPawnChar::Move);
        if (TiInputConfig->LookAction)
            EIC->BindAction(TiInputConfig->LookAction, ETriggerEvent::Triggered, this, &ATiPawnChar::Look);
        if (TiInputConfig->JumpAction)
        {
            EIC->BindAction(TiInputConfig->JumpAction, ETriggerEvent::Started, this, &ATiPawnChar::JumpAction);
            EIC->BindAction(TiInputConfig->JumpAction, ETriggerEvent::Completed, this, &ATiPawnChar::JumpActionReleased);
        }
    }
}

void ATiPawnChar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMovementModeCache();
    ApplyRotation(DeltaTime);
}

void ATiPawnChar::Move(const FInputActionValue& Value)
{
    if (Controller)
    {
        FVector2D MoveInput = Value.Get<FVector2D>();
        const FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Fwd = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Fwd * MoveInput.Y + Right * MoveInput.X);
    }
}

void ATiPawnChar::Look(const FInputActionValue& Value)
{
    if (Controller)
    {
        FVector2D LookInput = Value.Get<FVector2D>();
        AddControllerYawInput(LookInput.X);
        AddControllerPitchInput(LookInput.Y);
    }
}

void ATiPawnChar::JumpAction(const FInputActionValue& Value)
{
    if (TiPawnMovement) TiPawnMovement->SetJumpPressed(true);
}

void ATiPawnChar::JumpActionReleased(const FInputActionValue& Value)
{
    if (TiPawnMovement) TiPawnMovement->SetJumpPressed(false);
}

void ATiPawnChar::ApplyRotation(float DeltaTime)
{
    if (!Controller) return;

    FVector Input = GetLastMovementInputVector();
    if (Input.SizeSquared() > 0.1f)
    {
        FRotator Target = Input.ToOrientationRotator();
        FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Target, DeltaTime, 10.0f);
        NewRot.Pitch = 0;
        NewRot.Roll = 0;
        SetActorRotation(NewRot);
    }
}

// --- Helper Functions ---

ETiMovementMode ATiPawnChar::GetCurrentMovementMode() const
{
    if (TiPawnMovement)
    {
        return TiPawnMovement->GetTiMovementMode();
    }
    return ETiMovementMode::MOVE_None;
}

bool ATiPawnChar::IsMoving() const
{
    if (!TiPawnMovement) return false;
    return TiPawnMovement->Velocity.Size2D() > 10.0f;
}

bool ATiPawnChar::IsFalling() const
{
    if (!TiPawnMovement) return false;
    return TiPawnMovement->GetTiMovementMode() == ETiMovementMode::MOVE_Falling;
}

bool ATiPawnChar::IsWalking() const
{
    if (!TiPawnMovement) return false;
    return TiPawnMovement->GetTiMovementMode() == ETiMovementMode::MOVE_Walking && IsMoving();
}

void ATiPawnChar::UpdateMovementModeCache()
{
    CurrentMovementMode = GetCurrentMovementMode();
}
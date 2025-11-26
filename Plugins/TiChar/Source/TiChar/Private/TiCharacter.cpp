// Fill out your copyright notice in the Description page of Project Settings.

#include "TiCharacter.h"
#include "Components/InputComponent.h"  // For input bindings
#include "GameFramework/CharacterMovementComponent.h"  // For movement (if overriding)

// Sets default values
ATiCharacter::ATiCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)   // ✅ ensures ACharacter builds its Capsule, Movement, Mesh, etc.
{
    PrimaryActorTick.bCanEverTick = true;

    
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

    // ✅ Creates spring arm
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TiSpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.0f;
    SpringArm->bUsePawnControlRotation = true;

    // ✅ Creates camera
    TiCharCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TiCamera"));
    TiCharCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    TiCharCamera->bUsePawnControlRotation = false;

    // ❓ Still have ACharacter::Mesh (the built-in one) hanging around
}


// Called when the game starts or when spawned
void ATiCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ATiCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);  // Safety check

    // Movement bindings (left/right with MoveRight, forward with MoveForward)
    PlayerInputComponent->BindAxis("MoveForward", this, &ATiCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATiCharacter::MoveRight);

    // View/Turn bindings (turn right/left with Turn axis, look up/down with LookUp)
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);  // Rotates character/view right
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);  // Looks up/down

    // Optional: Jump action
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

// Forward movement (W/S keys or left stick Y)
void ATiCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        // Apply forward/backward movement
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

// Left/Right strafing (A/D keys or left stick X)
void ATiCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        // Apply right/left movement (positive Value = right)
        AddMovementInput(GetActorRightVector(), Value);
    }
}
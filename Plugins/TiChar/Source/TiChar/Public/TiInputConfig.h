#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TiInputConfig.generated.h"

UCLASS(BlueprintType)
class TICHAR_API UTiInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // ===== MAPPING CONTEXTS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* DrivingMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* FlyingMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* SwimmingMappingContext;

    // ===== CHARACTER MOVEMENT ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* WalkAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* ProneAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* SlideAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* VaultAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* MantleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Movement")
    UInputAction* ClimbAction;

    // ===== CHARACTER COMBAT ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* PrimaryFireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* SecondaryFireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* ReloadAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* MeleeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* GrenadeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Combat")
    UInputAction* SwitchWeaponAction;

    // ===== CHARACTER ABILITIES =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Abilities")
    UInputAction* Ability1Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Abilities")
    UInputAction* Ability2Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Abilities")
    UInputAction* Ability3Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Abilities")
    UInputAction* UltimateAction;

    // ===== CHARACTER INTERACTION =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Interaction")
    UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Interaction")
    UInputAction* UseAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Interaction")
    UInputAction* PickupAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Character|Interaction")
    UInputAction* DropAction;

    // ===== VEHICLE MOVEMENT ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* VehicleAccelerateAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* VehicleBrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* VehicleSteerAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* VehicleHandbrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* VehicleReverseAction;

    // ===== AIR VEHICLE SPECIFIC =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* VehiclePitchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* VehicleRollAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* VehicleYawAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* VehicleThrottleAction;

    // ===== VEHICLE SYSTEMS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* VehicleBoostAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* VehicleHeadlightsAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* VehicleHornAction;

    // ===== VEHICLE INTERACTION =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Interaction")
    UInputAction* VehicleEnterExitAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Interaction")
    UInputAction* VehicleSwitchSeatAction;

    // ===== VEHICLE COMBAT =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehiclePrimaryFireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehicleSecondaryFireAction;

    // ===== VEHICLE CAMERA =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Camera")
    UInputAction* VehicleLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Camera")
    UInputAction* VehicleSwitchCameraAction;

    // ===== MENU/UI ACTIONS (Shared) =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|UI")
    UInputAction* PauseAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|UI")
    UInputAction* InventoryAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|UI")
    UInputAction* MapAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|UI")
    UInputAction* ScoreboardAction;

    // ===== GADGET ACTIONS (Shared) =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Gadgets")
    UInputAction* Gadget1Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Gadgets")
    UInputAction* Gadget2Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Gadgets")
    UInputAction* Gadget3Action;

    // ===== COMMUNICATION ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Communication")
    UInputAction* EmoteWheelAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Communication")
    UInputAction* VoiceChatAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Communication")
    UInputAction* QuickChatAction;

    // ===== PHOTO MODE =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|PhotoMode")
    UInputAction* PhotoModeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|PhotoMode")
    UInputAction* HideUI_Action;

    // ===== DEBUG/DEVELOPER ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Debug")
    UInputAction* DebugAction1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Debug")
    UInputAction* DebugAction2;
};
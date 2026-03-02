#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TiInputConfig_Vehicle.generated.h"

UCLASS(BlueprintType)
class TICHAR_API UTiInputConfig_Vehicle : public UDataAsset
{
    GENERATED_BODY()

public:
    // Different mapping contexts for different vehicle types
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* GroundVehicleMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* AirVehicleMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* WaterVehicleMappingContext;

    // ===== VEHICLE MOVEMENT ACTIONS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* AccelerateAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* BrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* SteerAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* HandbrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Movement")
    UInputAction* ReverseAction;

    // ===== AIR VEHICLE SPECIFIC =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* PitchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* RollAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* YawAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* ThrottleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Air")
    UInputAction* AirBrakeAction;

    // ===== WATER VEHICLE SPECIFIC =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Water")
    UInputAction* DiveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Water")
    UInputAction* SurfaceAction;

    // ===== VEHICLE SYSTEMS =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* BoostAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* NitrousAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* HeadlightsAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* HornAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Systems")
    UInputAction* SirenAction;

    // ===== VEHICLE INTERACTION =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Interaction")
    UInputAction* EnterExitAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Interaction")
    UInputAction* SwitchSeatAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Interaction")
    UInputAction* ToggleDoorsAction;

    // ===== VEHICLE COMBAT =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehiclePrimaryFireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehicleSecondaryFireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehicleReloadAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehicleAbility1Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Combat")
    UInputAction* VehicleAbility2Action;

    // ===== VEHICLE CAMERA =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Camera")
    UInputAction* VehicleLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Camera")
    UInputAction* SwitchCameraViewAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Camera")
    UInputAction* LookBehindAction;

    // ===== VEHICLE MISC =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Misc")
    UInputAction* ToggleEngineAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Misc")
    UInputAction* RepairAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|Misc")
    UInputAction* EjectAction;

    // ===== VEHICLE UI =====
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|UI")
    UInputAction* VehicleMapAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Vehicle|UI")
    UInputAction* VehicleRadarAction;
};
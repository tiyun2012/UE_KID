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
    // Mapping contexts
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* DrivingMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Contexts")
    UInputMappingContext* FlyingMappingContext;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* JumpAction;
};

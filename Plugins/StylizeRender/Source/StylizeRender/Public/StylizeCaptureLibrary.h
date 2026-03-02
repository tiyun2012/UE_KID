#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StylizeCaptureLibrary.generated.h"

UCLASS()
class STYLIZERENDER_API UStylizeCaptureLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Stylize|Capture")
	static FString BuildHighResShotCommand(FIntPoint Resolution, const FString& OptionalFileName);

	UFUNCTION(BlueprintCallable, Category = "Stylize|Capture", meta = (WorldContext = "WorldContextObject"))
	static bool CaptureHighResScreenshot(const UObject* WorldContextObject, FIntPoint Resolution, const FString& OptionalFileName);
};

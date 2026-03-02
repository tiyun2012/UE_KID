#pragma once

#include "CoreMinimal.h"
#include "StylizeManagerActor.h"

#include "StylizeLevelTestActor.generated.h"

class UStylizePresetDataAsset;

UCLASS(BlueprintType, Blueprintable)
class STYLIZERENDER_API AStylizeLevelTestActor : public AStylizeManagerActor
{
	GENERATED_BODY()

public:
	AStylizeLevelTestActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|LevelTest")
	TArray<TObjectPtr<UStylizePresetDataAsset>> TestPresets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|LevelTest")
	TArray<FIntPoint> TestResolutions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|LevelTest", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float DelayBetweenCapturesSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|LevelTest")
	bool bAppendTimestampSuffix = true;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Stylize|LevelTest")
	void RunLevelTestCaptureSuite();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Stylize|LevelTest")
	void StopLevelTestCaptureSuite();

protected:
	UFUNCTION()
	void ExecuteNextCapture();

private:
	struct FQueuedCapture
	{
		TObjectPtr<UStylizePresetDataAsset> Preset;
		FIntPoint Resolution = FIntPoint::ZeroValue;
		FString OutputName;
	};

	void BuildCaptureQueue();

	TArray<FQueuedCapture> PendingCaptures;

	int32 NextCaptureIndex = 0;

	FTimerHandle CaptureTimerHandle;
};

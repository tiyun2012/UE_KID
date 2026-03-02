#include "StylizeLevelTestActor.h"

#include "Engine/World.h"
#include "StylizeCaptureLibrary.h"
#include "StylizePresetDataAsset.h"
#include "StylizeRender.h"
#include "TimerManager.h"

AStylizeLevelTestActor::AStylizeLevelTestActor()
{
	TestResolutions.Add(FIntPoint(3840, 2160));
	TestResolutions.Add(FIntPoint(7680, 4320));
}

void AStylizeLevelTestActor::RunLevelTestCaptureSuite()
{
	StopLevelTestCaptureSuite();
	BuildCaptureQueue();
	ExecuteNextCapture();
}

void AStylizeLevelTestActor::StopLevelTestCaptureSuite()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CaptureTimerHandle);
	}

	PendingCaptures.Reset();
	NextCaptureIndex = 0;
}

void AStylizeLevelTestActor::ExecuteNextCapture()
{
	if (!PendingCaptures.IsValidIndex(NextCaptureIndex))
	{
		UE_LOG(LogStylizeRender, Log, TEXT("Stylize level test capture suite completed."));
		return;
	}

	const FQueuedCapture Capture = PendingCaptures[NextCaptureIndex];
	++NextCaptureIndex;

	if (Capture.Preset)
	{
		ApplyPreset(Capture.Preset);
	}
	else
	{
		ApplyActivePreset();
	}

	const bool bRequested = UStylizeCaptureLibrary::CaptureHighResScreenshot(this, Capture.Resolution, Capture.OutputName);
	UE_LOG(LogStylizeRender, Log, TEXT("Queued capture '%s' at %dx%d returned %s."),
		*Capture.OutputName,
		Capture.Resolution.X,
		Capture.Resolution.Y,
		bRequested ? TEXT("true") : TEXT("false"));

	if (PendingCaptures.IsValidIndex(NextCaptureIndex))
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(CaptureTimerHandle, this, &AStylizeLevelTestActor::ExecuteNextCapture, DelayBetweenCapturesSeconds, false);
		}
	}
}

void AStylizeLevelTestActor::BuildCaptureQueue()
{
	TArray<FIntPoint> Resolutions = TestResolutions;
	if (Resolutions.Num() == 0)
	{
		Resolutions.Add(DefaultCaptureResolution);
	}

	TArray<TObjectPtr<UStylizePresetDataAsset>> Presets = TestPresets;
	if (Presets.Num() == 0)
	{
		Presets.Add(ActivePreset);
	}

	const FString TimestampSuffix = bAppendTimestampSuffix ? FString::Printf(TEXT("_%s"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"))) : FString();

	for (UStylizePresetDataAsset* Preset : Presets)
	{
		const FString PresetName = Preset ? Preset->GetName() : TEXT("NoPreset");

		for (const FIntPoint Resolution : Resolutions)
		{
			FQueuedCapture& Entry = PendingCaptures.AddDefaulted_GetRef();
			Entry.Preset = Preset;
			Entry.Resolution = FIntPoint(FMath::Max(1, Resolution.X), FMath::Max(1, Resolution.Y));
			Entry.OutputName = FString::Printf(
				TEXT("%s_%s_%dx%d%s"),
				*CaptureBaseName,
				*PresetName,
				Entry.Resolution.X,
				Entry.Resolution.Y,
				*TimestampSuffix
			);
		}
	}

	NextCaptureIndex = 0;

	UE_LOG(LogStylizeRender, Log, TEXT("Built stylize capture queue with %d entries."), PendingCaptures.Num());
}

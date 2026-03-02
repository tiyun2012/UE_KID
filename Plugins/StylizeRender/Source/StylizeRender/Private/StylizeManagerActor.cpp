#include "StylizeManagerActor.h"

#include "StylizeCaptureLibrary.h"
#include "StylizePresetDataAsset.h"
#include "StylizeRender.h"
#include "StylizeRenderSubsystem.h"

AStylizeManagerActor::AStylizeManagerActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AStylizeManagerActor::BeginPlay()
{
	Super::BeginPlay();

	if (bApplyOnBeginPlay)
	{
		ApplyActivePreset();
	}
}

bool AStylizeManagerActor::ApplyActivePreset()
{
	UStylizeRenderSubsystem* Subsystem = GetStylizeSubsystem();
	if (!Subsystem)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyActivePreset failed: subsystem unavailable."));
		return false;
	}

	if (!ActivePreset || !MaterialParameterCollection)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyActivePreset failed: missing active preset or material parameter collection."));
		return false;
	}

	Subsystem->SetMaterialParameterCollection(MaterialParameterCollection);
	return Subsystem->ApplyPreset(ActivePreset);
}

bool AStylizeManagerActor::ApplyPreset(UStylizePresetDataAsset* NewPreset)
{
	ActivePreset = NewPreset;
	return ApplyActivePreset();
}

bool AStylizeManagerActor::CaptureCurrentView()
{
	const FString CaptureName = BuildTimestampedCaptureName();
	const bool bRequested = UStylizeCaptureLibrary::CaptureHighResScreenshot(this, DefaultCaptureResolution, CaptureName);

	UE_LOG(LogStylizeRender, Log, TEXT("Capture request '%s' at %dx%d returned %s."),
		*CaptureName,
		DefaultCaptureResolution.X,
		DefaultCaptureResolution.Y,
		bRequested ? TEXT("true") : TEXT("false"));

	return bRequested;
}

UStylizeRenderSubsystem* AStylizeManagerActor::GetStylizeSubsystem() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UStylizeRenderSubsystem>() : nullptr;
}

FString AStylizeManagerActor::BuildTimestampedCaptureName() const
{
	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	return FString::Printf(TEXT("%s_%s"), *CaptureBaseName, *Timestamp);
}

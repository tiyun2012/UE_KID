#include "StylizeCaptureLibrary.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FString UStylizeCaptureLibrary::BuildHighResShotCommand(FIntPoint Resolution, const FString& OptionalFileName)
{
	const int32 SafeWidth = FMath::Max(1, Resolution.X);
	const int32 SafeHeight = FMath::Max(1, Resolution.Y);

	if (OptionalFileName.IsEmpty())
	{
		return FString::Printf(TEXT("HighResShot %dx%d"), SafeWidth, SafeHeight);
	}

	return FString::Printf(TEXT("HighResShot %dx%d filename=\"%s\""), SafeWidth, SafeHeight, *OptionalFileName);
}

bool UStylizeCaptureLibrary::CaptureHighResScreenshot(const UObject* WorldContextObject, FIntPoint Resolution, const FString& OptionalFileName)
{
	if (!GEngine || !WorldContextObject)
	{
		return false;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return false;
	}

	const FString Command = BuildHighResShotCommand(Resolution, OptionalFileName);

	if (APlayerController* PlayerController = World->GetFirstPlayerController())
	{
		PlayerController->ConsoleCommand(Command, true);
		return true;
	}

	return GEngine->Exec(World, *Command);
}

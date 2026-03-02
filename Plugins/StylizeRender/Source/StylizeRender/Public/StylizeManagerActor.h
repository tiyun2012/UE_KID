#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StylizeManagerActor.generated.h"

class UMaterialParameterCollection;
class UStylizePresetDataAsset;
class UStylizeRenderSubsystem;

UCLASS(BlueprintType, Blueprintable)
class STYLIZERENDER_API AStylizeManagerActor : public AActor
{
	GENERATED_BODY()

public:
	AStylizeManagerActor();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	TObjectPtr<UMaterialParameterCollection> MaterialParameterCollection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	TObjectPtr<UStylizePresetDataAsset> ActivePreset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	bool bApplyOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Capture")
	FIntPoint DefaultCaptureResolution = FIntPoint(3840, 2160);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Capture")
	FString CaptureBaseName = TEXT("StylizeCapture");

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Stylize")
	bool ApplyActivePreset();

	UFUNCTION(BlueprintCallable, Category = "Stylize")
	bool ApplyPreset(UStylizePresetDataAsset* NewPreset);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Stylize|Capture")
	bool CaptureCurrentView();

protected:
	UStylizeRenderSubsystem* GetStylizeSubsystem() const;
	FString BuildTimestampedCaptureName() const;
};

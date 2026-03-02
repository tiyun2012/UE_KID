#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "StylizeRenderSubsystem.generated.h"

class UMaterialParameterCollection;
class UStylizePresetDataAsset;

UCLASS()
class STYLIZERENDER_API UStylizeRenderSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Stylize")
	void SetMaterialParameterCollection(UMaterialParameterCollection* InCollection);

	UFUNCTION(BlueprintPure, Category = "Stylize")
	UMaterialParameterCollection* GetMaterialParameterCollection() const;

	UFUNCTION(BlueprintCallable, Category = "Stylize")
	bool ApplyPreset(UStylizePresetDataAsset* Preset);

	UFUNCTION(BlueprintCallable, Category = "Stylize")
	bool ApplyPresetToCollection(UStylizePresetDataAsset* Preset, UMaterialParameterCollection* InCollection);

	UFUNCTION(BlueprintPure, Category = "Stylize")
	UStylizePresetDataAsset* GetActivePreset() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialParameterCollection> MaterialParameterCollection;

	UPROPERTY(Transient)
	TObjectPtr<UStylizePresetDataAsset> ActivePreset;
};

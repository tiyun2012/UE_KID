#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "StylizePresetDataAsset.generated.h"

UCLASS(BlueprintType)
class STYLIZERENDER_API UStylizePresetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	float OutlineThickness = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize", meta = (ClampMin = "1", UIMin = "1"))
	int32 ColorQuantizationSteps = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	float ShadowContrast = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	FLinearColor ShadowTint = FLinearColor(0.03f, 0.04f, 0.06f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize")
	float PaperNoiseStrength = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Parameters")
	FName OutlineThicknessParameterName = TEXT("Stylize_OutlineThickness");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Parameters")
	FName ColorQuantizationStepsParameterName = TEXT("Stylize_ColorSteps");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Parameters")
	FName ShadowContrastParameterName = TEXT("Stylize_ShadowContrast");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Parameters")
	FName PaperNoiseStrengthParameterName = TEXT("Stylize_PaperNoise");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Parameters")
	FName ShadowTintParameterName = TEXT("Stylize_ShadowTint");
};

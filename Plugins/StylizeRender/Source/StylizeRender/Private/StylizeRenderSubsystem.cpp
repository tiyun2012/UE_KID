#include "StylizeRenderSubsystem.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "StylizePresetDataAsset.h"
#include "StylizeRender.h"

void UStylizeRenderSubsystem::SetMaterialParameterCollection(UMaterialParameterCollection* InCollection)
{
	MaterialParameterCollection = InCollection;
}

UMaterialParameterCollection* UStylizeRenderSubsystem::GetMaterialParameterCollection() const
{
	return MaterialParameterCollection;
}

bool UStylizeRenderSubsystem::ApplyPreset(UStylizePresetDataAsset* Preset)
{
	return ApplyPresetToCollection(Preset, MaterialParameterCollection);
}

bool UStylizeRenderSubsystem::ApplyPresetToCollection(UStylizePresetDataAsset* Preset, UMaterialParameterCollection* InCollection)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyPresetToCollection failed: invalid world."));
		return false;
	}

	if (!Preset || !InCollection)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyPresetToCollection failed: missing preset or material parameter collection."));
		return false;
	}

	const auto HasScalarParameter = [InCollection](const FName ParameterName)
	{
		return InCollection->ScalarParameters.ContainsByPredicate(
			[ParameterName](const FCollectionScalarParameter& Parameter)
			{
				return Parameter.ParameterName == ParameterName;
			});
	};

	const auto HasVectorParameter = [InCollection](const FName ParameterName)
	{
		return InCollection->VectorParameters.ContainsByPredicate(
			[ParameterName](const FCollectionVectorParameter& Parameter)
			{
				return Parameter.ParameterName == ParameterName;
			});
	};

	if (!HasScalarParameter(Preset->OutlineThicknessParameterName)
		|| !HasScalarParameter(Preset->ColorQuantizationStepsParameterName)
		|| !HasScalarParameter(Preset->ShadowContrastParameterName)
		|| !HasScalarParameter(Preset->PaperNoiseStrengthParameterName)
		|| !HasVectorParameter(Preset->ShadowTintParameterName))
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyPresetToCollection failed: collection '%s' is missing one or more required parameters."), *GetNameSafe(InCollection));
		return false;
	}

	UMaterialParameterCollectionInstance* CollectionInstance = World->GetParameterCollectionInstance(InCollection);
	if (!CollectionInstance)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("ApplyPresetToCollection failed: could not acquire collection instance '%s'."), *GetNameSafe(InCollection));
		return false;
	}

	CollectionInstance->SetScalarParameterValue(Preset->OutlineThicknessParameterName, Preset->OutlineThickness);
	CollectionInstance->SetScalarParameterValue(Preset->ColorQuantizationStepsParameterName, static_cast<float>(Preset->ColorQuantizationSteps));
	CollectionInstance->SetScalarParameterValue(Preset->ShadowContrastParameterName, Preset->ShadowContrast);
	CollectionInstance->SetScalarParameterValue(Preset->PaperNoiseStrengthParameterName, Preset->PaperNoiseStrength);
	CollectionInstance->SetVectorParameterValue(Preset->ShadowTintParameterName, Preset->ShadowTint);

	ActivePreset = Preset;
	MaterialParameterCollection = InCollection;

	UE_LOG(LogStylizeRender, Log, TEXT("Applied stylize preset '%s' using MPC '%s'."), *GetNameSafe(Preset), *GetNameSafe(InCollection));
	return true;
}

UStylizePresetDataAsset* UStylizeRenderSubsystem::GetActivePreset() const
{
	return ActivePreset;
}

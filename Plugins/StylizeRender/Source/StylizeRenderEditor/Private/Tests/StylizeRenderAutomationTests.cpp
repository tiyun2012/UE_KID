#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Editor.h"
#include "Materials/MaterialParameterCollection.h"
#include "StylizeCaptureLibrary.h"
#include "StylizePresetDataAsset.h"
#include "StylizeRenderSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStylizePresetDefaultsAutomationTest,
	"KIDs.Stylize.PresetDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStylizePresetDefaultsAutomationTest::RunTest(const FString& Parameters)
{
	UStylizePresetDataAsset* Preset = NewObject<UStylizePresetDataAsset>(GetTransientPackage());
	TestNotNull(TEXT("Preset object is created"), Preset);
	if (!Preset)
	{
		return false;
	}

	TestTrue(TEXT("Outline thickness is non-negative"), Preset->OutlineThickness >= 0.0f);
	TestTrue(TEXT("Color quantization is at least one step"), Preset->ColorQuantizationSteps >= 1);
	TestTrue(TEXT("Shadow contrast is non-negative"), Preset->ShadowContrast >= 0.0f);
	TestTrue(TEXT("Paper noise strength is non-negative"), Preset->PaperNoiseStrength >= 0.0f);
	TestFalse(TEXT("Outline parameter name is not empty"), Preset->OutlineThicknessParameterName.IsNone());
	TestFalse(TEXT("Shadow tint parameter name is not empty"), Preset->ShadowTintParameterName.IsNone());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStylizeCaptureCommandAutomationTest,
	"KIDs.Stylize.CaptureCommand",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStylizeCaptureCommandAutomationTest::RunTest(const FString& Parameters)
{
	const FString ExplicitFileCommand = UStylizeCaptureLibrary::BuildHighResShotCommand(FIntPoint(3840, 2160), TEXT("Stylize_4K"));
	TestEqual(TEXT("Builds command with file name"), ExplicitFileCommand, FString(TEXT("HighResShot 3840x2160 filename=\"Stylize_4K\"")));

	const FString SanitizedCommand = UStylizeCaptureLibrary::BuildHighResShotCommand(FIntPoint(-2, 0), FString());
	TestEqual(TEXT("Sanitizes invalid dimensions"), SanitizedCommand, FString(TEXT("HighResShot 1x1")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStylizeSubsystemAutomationTest,
	"KIDs.Stylize.SubsystemSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStylizeSubsystemAutomationTest::RunTest(const FString& Parameters)
{
	if (!GEditor)
	{
		AddWarning(TEXT("GEditor is unavailable; skipping subsystem safety validation."));
		return true;
	}

	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		AddWarning(TEXT("Editor world is unavailable; skipping subsystem safety validation."));
		return true;
	}

	UStylizeRenderSubsystem* Subsystem = EditorWorld->GetSubsystem<UStylizeRenderSubsystem>();
	TestNotNull(TEXT("Stylize world subsystem exists"), Subsystem);
	if (!Subsystem)
	{
		return false;
	}

	UStylizePresetDataAsset* Preset = NewObject<UStylizePresetDataAsset>(GetTransientPackage());
	TestNotNull(TEXT("Temporary preset can be created"), Preset);
	if (!Preset)
	{
		return false;
	}

	UMaterialParameterCollection* TestCollection = NewObject<UMaterialParameterCollection>(GetTransientPackage());
	TestNotNull(TEXT("Temporary MPC can be created"), TestCollection);
	if (!TestCollection)
	{
		return false;
	}

	FCollectionScalarParameter& Outline = TestCollection->ScalarParameters.AddDefaulted_GetRef();
	Outline.ParameterName = Preset->OutlineThicknessParameterName;

	FCollectionScalarParameter& Steps = TestCollection->ScalarParameters.AddDefaulted_GetRef();
	Steps.ParameterName = Preset->ColorQuantizationStepsParameterName;

	FCollectionScalarParameter& Contrast = TestCollection->ScalarParameters.AddDefaulted_GetRef();
	Contrast.ParameterName = Preset->ShadowContrastParameterName;

	FCollectionScalarParameter& Noise = TestCollection->ScalarParameters.AddDefaulted_GetRef();
	Noise.ParameterName = Preset->PaperNoiseStrengthParameterName;

	FCollectionVectorParameter& Tint = TestCollection->VectorParameters.AddDefaulted_GetRef();
	Tint.ParameterName = Preset->ShadowTintParameterName;

	const bool bApplied = Subsystem->ApplyPresetToCollection(Preset, TestCollection);
	TestTrue(TEXT("Applying with required MPC parameters succeeds"), bApplied);
	TestEqual(TEXT("Active preset is updated"), Subsystem->GetActivePreset(), Preset);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

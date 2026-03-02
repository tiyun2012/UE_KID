#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Editor.h"
#include "Materials/MaterialParameterCollection.h"
#include "StylizeCaptureLibrary.h"
#include "StylizePresetDataAsset.h"
#include "StylizeRenderSubsystem.h"
#include "StylizeWaterActor.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStylizeWaterActorStateAutomationTest,
	"KIDs.Stylize.WaterActorStateCache",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStylizeWaterActorStateAutomationTest::RunTest(const FString& Parameters)
{
	if (!GEditor)
	{
		AddWarning(TEXT("GEditor is unavailable; skipping water actor state test."));
		return true;
	}

	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		AddWarning(TEXT("Editor world is unavailable; skipping water actor state test."));
		return true;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;

	AStylizeWaterActor* WaterActor = EditorWorld->SpawnActor<AStylizeWaterActor>(AStylizeWaterActor::StaticClass(), FTransform::Identity, SpawnParameters);
	TestNotNull(TEXT("Water actor can be spawned"), WaterActor);
	if (!WaterActor)
	{
		return false;
	}

	WaterActor->InitializeSimulation();
	TestNotNull(TEXT("Simulation render target is initialized"), WaterActor->GetCurrentStateRenderTarget());

	WaterActor->UpdateBoatPhysics(FVector2D(0.25f, 0.75f), 0.4f, 0.9f);
	const FVector4 BoatData = WaterActor->GetCurrentBoatData();
	TestTrue(TEXT("Boat U is updated"), FMath::IsNearlyEqual(BoatData.X, 0.25f));
	TestTrue(TEXT("Boat V is updated"), FMath::IsNearlyEqual(BoatData.Y, 0.75f));
	TestTrue(TEXT("Boat heading is updated"), FMath::IsNearlyEqual(BoatData.Z, 0.4f));
	TestTrue(TEXT("Boat speed is updated"), FMath::IsNearlyEqual(BoatData.W, 0.9f));

	WaterActor->UpdateObstacle(1, FVector2D(0.1f, 0.2f), 0.05f);
	const FVector Obstacle1 = WaterActor->GetObstacleData(1);
	TestTrue(TEXT("Obstacle U is updated"), FMath::IsNearlyEqual(Obstacle1.X, 0.1f));
	TestTrue(TEXT("Obstacle V is updated"), FMath::IsNearlyEqual(Obstacle1.Y, 0.2f));
	TestTrue(TEXT("Obstacle radius is updated"), FMath::IsNearlyEqual(Obstacle1.Z, 0.05f));

	const FVector Obstacle0Before = WaterActor->GetObstacleData(0);
	WaterActor->UpdateObstacle(10, FVector2D(1.0f, 1.0f), 1.0f);
	const FVector Obstacle0After = WaterActor->GetObstacleData(0);
	TestTrue(TEXT("Invalid obstacle index does not modify cached obstacles"), Obstacle0Before.Equals(Obstacle0After));

	WaterActor->Destroy();
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

#include "StylizeWaterActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "StylizeRender.h"
#include "UObject/ConstructorHelpers.h"

AStylizeWaterActor::AStylizeWaterActor()
{
	PrimaryActorTick.bCanEverTick = true;

	WaterSurfaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterSurfaceMesh"));
	SetRootComponent(WaterSurfaceMesh);

	WaterSurfaceMesh->SetMobility(EComponentMobility::Movable);
	WaterSurfaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMeshFinder.Succeeded())
	{
		WaterSurfaceMesh->SetStaticMesh(PlaneMeshFinder.Object);
		WaterSurfaceMesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 1.0f));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SimMatFinder(TEXT("/Game/StylizeRender/Materials/M_WaterSim_Template.M_WaterSim_Template"));
	if (SimMatFinder.Succeeded())
	{
		SimulationMaterialTemplate = SimMatFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SurfaceMatFinder(TEXT("/Game/StylizeRender/Materials/M_WaterSurface_Template.M_WaterSurface_Template"));
	if (SurfaceMatFinder.Succeeded())
	{
		WaterSurfaceMaterialTemplate = SurfaceMatFinder.Object;
	}

	ObstacleData.Init(FVector::ZeroVector, 3);
	CurrentBoatData = FVector4(-100.0f, -100.0f, 0.0f, 0.0f);
}

void AStylizeWaterActor::BeginPlay()
{
	Super::BeginPlay();

	if (!SimulationMaterialTemplate)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("AStylizeWaterActor '%s' is missing SimulationMaterialTemplate."), *GetName());
	}

	if (!WaterSurfaceMaterialTemplate)
	{
		UE_LOG(LogStylizeRender, Warning, TEXT("AStylizeWaterActor '%s' is missing WaterSurfaceMaterialTemplate."), *GetName());
	}

	InitializeSimulation();
}

void AStylizeWaterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SimulationMID || !CurrentStateRT || !NextStateRT)
	{
		return;
	}

	const float ClampedDt = FMath::Min(DeltaTime, 0.05f);
	SimulationMID->SetTextureParameterValue(TEXT("CurrentState"), CurrentStateRT);
	SimulationMID->SetScalarParameterValue(TEXT("Dt"), ClampedDt);
	SimulationMID->SetScalarParameterValue(TEXT("Viscosity"), Viscosity);
	SimulationMID->SetVectorParameterValue(TEXT("BoatData"),
		FLinearColor(CurrentBoatData.X, CurrentBoatData.Y, CurrentBoatData.Z, CurrentBoatData.W));

	for (int32 ObstacleIndex = 0; ObstacleIndex < ObstacleData.Num(); ++ObstacleIndex)
	{
		const FName ParameterName = *FString::Printf(TEXT("Obstacle%d"), ObstacleIndex);
		const FVector& Obstacle = ObstacleData[ObstacleIndex];
		SimulationMID->SetVectorParameterValue(ParameterName, FLinearColor(Obstacle.X, Obstacle.Y, Obstacle.Z, 1.0f));
	}

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), NextStateRT, SimulationMID);
	SwapRenderTargets();

	if (WaterSurfaceMID)
	{
		WaterSurfaceMID->SetTextureParameterValue(TEXT("SimulationTexture"), CurrentStateRT);
		WaterSurfaceMID->SetScalarParameterValue(TEXT("WaveAmplitude"), WaveAmplitude);
		WaterSurfaceMID->SetScalarParameterValue(TEXT("FoamIntensity"), FoamIntensity);
	}
}

void AStylizeWaterActor::UpdateBoatPhysics(FVector2D UVPosition, float Heading, float SpeedNorm)
{
	CurrentBoatData = FVector4(UVPosition.X, UVPosition.Y, Heading, SpeedNorm);
}

void AStylizeWaterActor::UpdateObstacle(int32 Index, FVector2D UVPosition, float RadiusUV)
{
	if (ObstacleData.IsValidIndex(Index))
	{
		ObstacleData[Index] = FVector(UVPosition.X, UVPosition.Y, RadiusUV);
	}
}

FVector4 AStylizeWaterActor::GetCurrentBoatData() const
{
	return CurrentBoatData;
}

FVector AStylizeWaterActor::GetObstacleData(int32 Index) const
{
	return ObstacleData.IsValidIndex(Index) ? ObstacleData[Index] : FVector::ZeroVector;
}

UTextureRenderTarget2D* AStylizeWaterActor::GetCurrentStateRenderTarget() const
{
	return CurrentStateRT;
}

void AStylizeWaterActor::InitializeSimulation()
{
	InitializeRenderTargets();

	if (SimulationMaterialTemplate)
	{
		SimulationMID = UMaterialInstanceDynamic::Create(SimulationMaterialTemplate, this);
	}
	else
	{
		SimulationMID = nullptr;
	}

	if (WaterSurfaceMaterialTemplate && WaterSurfaceMesh)
	{
		WaterSurfaceMID = UMaterialInstanceDynamic::Create(WaterSurfaceMaterialTemplate, this);
		WaterSurfaceMesh->SetMaterial(0, WaterSurfaceMID);
		WaterSurfaceMID->SetScalarParameterValue(TEXT("WaveAmplitude"), WaveAmplitude);
		WaterSurfaceMID->SetScalarParameterValue(TEXT("FoamIntensity"), FoamIntensity);
	}
	else
	{
		WaterSurfaceMID = nullptr;
	}
}

void AStylizeWaterActor::InitializeRenderTargets()
{
	const int32 SafeResolution = FMath::Max(64, SimulationResolution);
	if (CurrentStateRT
		&& NextStateRT
		&& CurrentStateRT->SizeX == SafeResolution
		&& CurrentStateRT->SizeY == SafeResolution
		&& NextStateRT->SizeX == SafeResolution
		&& NextStateRT->SizeY == SafeResolution)
	{
		return;
	}

	CurrentStateRT = UKismetRenderingLibrary::CreateRenderTarget2D(
		this,
		SafeResolution,
		SafeResolution,
		RTF_RGBA16f,
		FLinearColor::Transparent,
		false);

	NextStateRT = UKismetRenderingLibrary::CreateRenderTarget2D(
		this,
		SafeResolution,
		SafeResolution,
		RTF_RGBA16f,
		FLinearColor::Transparent,
		false);

	if (CurrentStateRT && NextStateRT)
	{
		CurrentStateRT->AddressX = TA_Clamp;
		CurrentStateRT->AddressY = TA_Clamp;
		NextStateRT->AddressX = TA_Clamp;
		NextStateRT->AddressY = TA_Clamp;

		UKismetRenderingLibrary::ClearRenderTarget2D(this, CurrentStateRT, FLinearColor::Transparent);
		UKismetRenderingLibrary::ClearRenderTarget2D(this, NextStateRT, FLinearColor::Transparent);
	}
}

void AStylizeWaterActor::SwapRenderTargets()
{
	UTextureRenderTarget2D* TempRT = CurrentStateRT;
	CurrentStateRT = NextStateRT;
	NextStateRT = TempRT;
}

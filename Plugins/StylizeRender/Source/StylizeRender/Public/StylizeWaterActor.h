#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StylizeWaterActor.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

UCLASS(BlueprintType, Blueprintable)
class STYLIZERENDER_API AStylizeWaterActor : public AActor
{
	GENERATED_BODY()

public:
	AStylizeWaterActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Stylize|Water", meta = (DisplayName = "Initialize Simulation"))
	void InitializeSimulation();

	UFUNCTION(BlueprintCallable, Category = "Stylize|Water")
	void UpdateBoatPhysics(FVector2D UVPosition, float Heading, float SpeedNorm);

	UFUNCTION(BlueprintCallable, Category = "Stylize|Water")
	void UpdateObstacle(int32 Index, FVector2D UVPosition, float RadiusUV);

	UFUNCTION(BlueprintPure, Category = "Stylize|Water")
	FVector4 GetCurrentBoatData() const;

	UFUNCTION(BlueprintPure, Category = "Stylize|Water")
	FVector GetObstacleData(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Stylize|Water")
	UTextureRenderTarget2D* GetCurrentStateRenderTarget() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stylize|Water")
	TObjectPtr<UStaticMeshComponent> WaterSurfaceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Materials")
	TObjectPtr<UMaterialInterface> SimulationMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Materials")
	TObjectPtr<UMaterialInterface> WaterSurfaceMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Settings", meta = (ClampMin = "64", UIMin = "256"))
	int32 SimulationResolution = 256;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Settings")
	float Viscosity = 0.98f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Visuals")
	float WaveAmplitude = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylize|Water|Visuals")
	float FoamIntensity = 2.0f;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SimulationMID;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> WaterSurfaceMID;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> CurrentStateRT;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> NextStateRT;

	FVector4 CurrentBoatData;
	TArray<FVector> ObstacleData;

	void InitializeRenderTargets();
	void SwapRenderTargets();
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

USTRUCT(BlueprintType)
struct FNoiseLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = NoiseLayer)
	int32 Seed = 1;

	UPROPERTY(EditAnywhere, Category = NoiseLayer,
		meta = (ClampMin = 1.f))
	float Scale = 1.f;

	UPROPERTY(EditAnywhere, Category = NoiseLayer)
	float PositionHeightOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = NoiseLayer)
	int32 Octaves = 1;

	UPROPERTY(EditAnywhere, Category = NoiseLayer)
	float Persistence = 1.f;

	// Recommended ~[0.3, 0.5] if octaves = 12
	// higher octaves = higher lacunarity
	UPROPERTY(EditAnywhere, Category = NoiseLayer,
		meta = (ClampMin = 0, ClampMax = 1))
	float Lacunarity = .2f;

	UPROPERTY(EditAnywhere, Category = NoiseLayer)
	float HeightThreshold = 0;
};

UCLASS()
class RTS_API AWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	AWorldGenerator();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Components)
	TObjectPtr<class UProceduralMeshComponent> TerrainMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "WorldGenerator|Terrain")
	int32 Resolution = 64;

	UPROPERTY(EditAnywhere, Category = "WorldGenerator|Terrain")
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, Category = "WorldGenerator|Terrain")
	int32 IsolationVertexFilterRadius = 10;

	UPROPERTY(EditAnywhere, Category = "WorldGenerator|Terrain")
	TArray<FNoiseLayer> NoiseLayers;

protected:
	UFUNCTION(BlueprintCallable, Category = WorldGenerator)
	virtual void GenerateMesh();

	virtual float GetHeight(const FVector2D& InLocation) const;
};

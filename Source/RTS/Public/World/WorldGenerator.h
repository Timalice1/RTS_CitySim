#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

UCLASS()
class RTS_API AWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	AWorldGenerator();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class UProceduralMeshComponent> TerrainMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = WorldGenerator)
	int32 TerrainSize = 50;

	UPROPERTY(EditAnywhere, Category = WorldGenerator)
	float CellSize = 100.f;

protected:
	virtual void GenerateMesh();

	virtual float GetHeight() const;
	
};

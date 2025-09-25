#include "World/WorldGenerator.h"
#include "ProceduralMeshComponent.h"

AWorldGenerator::AWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateMesh();
}

void AWorldGenerator::GenerateMesh()
{
	// Calculate square grid of vertices
	TArray<FVector> Vertices;
	for (int32 y = 0; y < TerrainSize; ++y)
	{
		for (int32 x = 0; x < TerrainSize; ++x)
			Vertices.Add(FVector(x * CellSize, y * CellSize, GetHeight()));
	}

	// Calculate triangles points
	TArray<int32> Triangles;
	for (int32 y = 0; y < TerrainSize - 1; ++y)
	{
		for (int32 x = 0; x < TerrainSize - 1; ++x)
		{
			int32 pointIndex = y * TerrainSize + x;
			//========ORDER MATTER!!!!===========
			// Fill first triangle of polygon
			Triangles.Add(pointIndex);
			Triangles.Add(pointIndex + TerrainSize);
			Triangles.Add(pointIndex + 1);

			// fill second triangle
			Triangles.Add(pointIndex + TerrainSize);
			Triangles.Add(pointIndex + TerrainSize + 1);
			Triangles.Add(pointIndex + 1);
		}
	}

	const TArray<FVector> normals;
	const TArray<FVector2D> uv;
	const TArray<FColor> vertexColors;
	const TArray<FProcMeshTangent> tangents;
	bool bCreateCollision = true;

	TerrainMesh->CreateMeshSection(0, Vertices, Triangles, normals, uv, vertexColors, tangents, bCreateCollision);
}

float AWorldGenerator::GetHeight() const
{
	return 0.f;
}

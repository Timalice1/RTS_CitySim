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
	// GenerateMesh();
}

void AWorldGenerator::GenerateMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uv;
	TArray<FProcMeshTangent> tangents;
	const TArray<FColor> vertexColors;
	bool bCreateCollision = true;
	// Calculate square grid of vertices
	for (int32 y = 0; y < Resolution; ++y)
	{
		for (int32 x = 0; x < Resolution; ++x)
		{
			const float xCoord = x * CellSize;
			const float yCoord = y * CellSize;
			const float zCoord = GetHeight(FVector2D(xCoord, yCoord));
			Vertices.Add(FVector(xCoord, yCoord, zCoord));

			// Calculate mesh UV's
			uv.Add(FVector2D(x, y) * (CellSize / 50));
		}
	}

	// Filter out isolated vertices
	for (int32 y = 0; y < Resolution; ++y)
	{
		for (int32 x = 0; x < Resolution; ++x)
		{
			const int32 index = y * Resolution + x;
			// Skip vertex if already zero
			if (!Vertices.IsValidIndex(index) || Vertices[index].Z <= 0)
				continue;

			int32 nonZeroNeighbours = 0;
			for (int32 ny = -IsolationVertexFilterRadius; ny <= IsolationVertexFilterRadius; ny++)
			{
				for (int32 nx = -IsolationVertexFilterRadius; nx <= IsolationVertexFilterRadius; nx++)
				{
					const int32 n_index = (ny + y) * Resolution + (nx + x);
					if (Vertices.IsValidIndex(n_index) && n_index != index && Vertices[n_index].Z > 0)
						nonZeroNeighbours++;
				}
			}

			const int32 step = (FMath::Square(IsolationVertexFilterRadius + IsolationVertexFilterRadius + 1) - 1) / 2;
			if (nonZeroNeighbours < step)
				Vertices[index].Z = 0;
		}
	}

	// Calculate triangles points
	for (int32 y = 0; y < Resolution - 1; ++y)
	{
		for (int32 x = 0; x < Resolution - 1; ++x)
		{
			int32 pointIndex = y * Resolution + x;
			//========ORDER MATTER!!!!===========
			// Fill first triangle of polygon
			Triangles.Add(pointIndex);
			Triangles.Add(pointIndex + Resolution);
			Triangles.Add(pointIndex + 1);

			// fill second triangle
			Triangles.Add(pointIndex + Resolution);
			Triangles.Add(pointIndex + Resolution + 1);
			Triangles.Add(pointIndex + 1);
		}
	}
	// UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, uv, normals, tangents);
	TerrainMesh->CreateMeshSection(0, Vertices, Triangles, normals, uv, vertexColors, tangents, bCreateCollision);
}

float AWorldGenerator::GetHeight(const FVector2D& InLocation) const
{
	float total = 0;
	for (const FNoiseLayer& layer : NoiseLayers)
	{
		float noise = 0;
		float amplitude = 0;
		for (int32 i = 0; i < layer.Octaves; i++)
		{
			const FVector2D position = InLocation / layer.Scale * FMath::Pow(layer.Lacunarity, i);
			const float perlinVal = FMath::PerlinNoise2D(position);
			noise += amplitude * perlinVal;
			amplitude += FMath::Pow(layer.Persistence, i);
			noise -= layer.PositionHeightOffset;
		}
		total += noise;
		if (total < layer.HeightThreshold)
			total = 0;
	}
	return total;
}


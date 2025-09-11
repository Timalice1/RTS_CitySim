#include "Buildables/RTS_BuildPreview.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInstance.h"

ARTS_BuildPreview::ARTS_BuildPreview()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Setup box collider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollider;
	BoxCollider->bHiddenInGame = false;
	BoxCollider->SetGenerateOverlapEvents(true);

	// Setup building mesh component
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	BuildingMesh->SetupAttachment(RootComponent);
	BuildingMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void ARTS_BuildPreview::InitBuilding(const FBuildingData& BuildingData)
{
	_buildingData = BuildingData;

	if (UStaticMesh* meshComplete = _buildingData.BuildingMesh_Complete.LoadSynchronous())
	{
		BuildingMesh->SetStaticMesh(meshComplete);

		// Create and apply dynamic overlay material instance
		if (UMaterialInstance* overlay = OverlayPreviewMaterial.LoadSynchronous())
		{
			_overlayMaterial = UMaterialInstanceDynamic::Create(overlay, this);
			if (_overlayMaterial)
				BuildingMesh->SetOverlayMaterial(_overlayMaterial);
		}

		// Init collider size
		FVector Min, Max;
		BuildingMesh->GetLocalBounds(Min, Max);

		// Building size adjustment to match world grid parameters
		// BuildingSize = MeshBounds + (CellSize - (MeshBounds % CellSize));
		FVector targetSize = Max;
		const uint32 CellSize = BuildingData.CellSize;
		if ((static_cast<int>(Max.X) % CellSize != 0) && (static_cast<int>(Max.Y) % CellSize != 0))
		{
			targetSize.X = Max.X + (CellSize - static_cast<int>(Max.X) % CellSize);
			targetSize.Y = Max.Y + (CellSize - static_cast<int>(Max.Y) % CellSize);
		}
		BoxCollider->SetBoxExtent(targetSize);
	}
}

void ARTS_BuildPreview::StartBuild()
{
	if (bIsPlaceable)
	{
		_buildProgress = 0;
		GetWorld()->GetTimerManager().SetTimer(_buildTimer, this, &ThisClass::UpdateBuildingProgress, _buildRate, true, 0);
		// Disable overlay material
		BuildingMesh->SetOverlayMaterial(nullptr);
	}
}

void ARTS_BuildPreview::UpdateBuildingProgress()
{
	if (_buildProgress >= 1)
	{
		if (UStaticMesh* completeMesh = _buildingData.BuildingMesh_Complete.LoadSynchronous())
			BuildingMesh->SetStaticMesh(completeMesh);
		EndBuild();
	}
	else
	{
		int32 progressMeshIndex = FMath::Floor(_buildProgress * _buildingData.BuildingMesh_Stages.Num());
		if (_buildingData.BuildingMesh_Stages.IsValidIndex(progressMeshIndex))
		{
			if (UStaticMesh* progressMesh = _buildingData.BuildingMesh_Stages[progressMeshIndex].LoadSynchronous())
				BuildingMesh->SetStaticMesh(progressMesh);
		}
	}

	_buildProgress += 1.f / _buildingData.BuildingMesh_Stages.Num();
}

void ARTS_BuildPreview::ValidatePlacement()
{
	TArray<AActor*> OverlappedActors;
	BoxCollider->GetOverlappingActors(OverlappedActors);
	bIsPlaceable = OverlappedActors.Num() == 0;
	if (_overlayMaterial)
		_overlayMaterial->SetScalarParameterValue(TEXT("Status"), bIsPlaceable ? 1.f : 0.f);
}

void ARTS_BuildPreview::EndBuild()
{
	GetWorld()->GetTimerManager().ClearTimer(_buildTimer);
	OnBuildCompleted.Broadcast(this);
}

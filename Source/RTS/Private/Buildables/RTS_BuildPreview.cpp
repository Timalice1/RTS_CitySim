#include "Buildables/RTS_BuildPreview.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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
	InitMeshAndCollider();
	ApplyOverlay();
}

void ARTS_BuildPreview::InitMeshAndCollider()
{
	if (!BuildingMesh || !BoxCollider)
		return;

	if (UStaticMesh* meshComplete = _buildingData.BuildingMesh_Complete.LoadSynchronous())
	{
		BuildingMesh->SetStaticMesh(meshComplete);

		// Init collider size
		FVector Min, Max;
		BuildingMesh->GetLocalBounds(Min, Max);
		buildingBounds = Min;

		// Building size adjustment to match world grid parameters
		// BuildingSize = MeshBounds + (CellSize - (MeshBounds % CellSize));
		const uint32 CellSize = _buildingData.CellSize;
		if ((static_cast<int>(Max.X) % CellSize != 0) && (static_cast<int>(Max.Y) % CellSize != 0))
		{
			buildingBounds.X = Max.X + (CellSize - static_cast<int>(Max.X) % CellSize);
			buildingBounds.Y = Max.Y + (CellSize - static_cast<int>(Max.Y) % CellSize);
		}
		//buildingBounds.Z *= .5f;
	}
}

void ARTS_BuildPreview::ApplyOverlay()
{
	// Create and apply dynamic overlay material instance
	if (UMaterialInstance* overlay = OverlayPreviewMaterial.LoadSynchronous())
	{
		_overlayMaterial = UMaterialInstanceDynamic::Create(overlay, this);
		if (_overlayMaterial)
			BuildingMesh->SetOverlayMaterial(_overlayMaterial);
	}
}

void ARTS_BuildPreview::StartBuild()
{
	_durabilityCurrent = 0.f;
	_buildProgress = 0.f;

	// TODO: Create new task in the tasks manager
	GetWorld()->GetTimerManager().SetTimer(_buildTimer, this, &ThisClass::UpdateBuildingProgress, GetWorld()->GetDeltaSeconds(), true, 0);
	// Disable overlay material
	BuildingMesh->SetOverlayMaterial(nullptr);
}

void ARTS_BuildPreview::UpdateBuildingProgress()
{
	if (!BuildingMesh)
		return;

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

	UKismetSystemLibrary::DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 1200.f, FString::Printf(TEXT("Progress: {%.3f}"), _buildProgress), NULL, FLinearColor::Yellow);

	// TODO: Save building progress, restore progress on load
	_durabilityCurrent += .1f;
	_buildProgress = _durabilityCurrent / _buildingData.MaxDurability;
}

void ARTS_BuildPreview::SetIsPlaceable(bool bIsPlaceable)
{
	if (_overlayMaterial)
		_overlayMaterial->SetScalarParameterValue(TEXT("Status"), bIsPlaceable ? 1.f : 0.f);
}

void ARTS_BuildPreview::SaveObjectData(FArchive& Ar) {}

void ARTS_BuildPreview::LoadObjectData(FArchive& Ar)
{
	InitMeshAndCollider();
	GetWorld()->GetTimerManager().SetTimer(_buildTimer, this, &ThisClass::UpdateBuildingProgress, GetWorld()->GetDeltaSeconds(), true, 0);
}

void ARTS_BuildPreview::EndBuild()
{
	GetWorld()->GetTimerManager().ClearTimer(_buildTimer);
	OnBuildCompleted.Broadcast(this);
}

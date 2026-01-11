#include "Buildables/RTS_BuildPreview.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstance.h"
#include "UI/Buildables/Building_InteractionPanel.h"
#include "UI/Generic/G_TextBlock.h"
#include "UI/Generic/InteractionPanelWidget.h"

ARTS_BuildPreview::ARTS_BuildPreview()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Setup building mesh component
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	BuildingMesh->SetupAttachment(RootComponent);
	BuildingMesh->SetCollisionProfileName(TEXT("BlockAll"));

	InteractionPanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPanelWidgetComponent"));
	InteractionPanelWidget->SetupAttachment(BuildingMesh);
	InteractionPanelWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionPanelWidget->SetDrawAtDesiredSize(true);
}

void ARTS_BuildPreview::BeginPlay()
{
	Super::BeginPlay();
	InteractionPanelWidget->SetVisibility(false);
}

void ARTS_BuildPreview::Init(const FBuildingData& BuildingData)
{
	_buildingData = BuildingData;
	InitBuilding();
	
	// Apply preview validation overlay material to the mesh
	ApplyOverlay();
}

void ARTS_BuildPreview::InitBuilding()
{
	if (!BuildingMesh)
		return;

	// Init mesh and mesh bounds
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
	}

	// Initialize interaction panel widget
	if (TSubclassOf<UInteractionPanelWidget> interactionPanelClass = InteractionPanelWidgetClass.LoadSynchronous())
	{
		if (UBuilding_InteractionPanel* interactionPanel = Cast<UBuilding_InteractionPanel>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), interactionPanelClass)))
		{
			interactionPanel->Title->SetText(_buildingData.Title);
			interactionPanel->OnDestroyButtonClicked.BindUObject(this, &ThisClass::HandleCancelBuild);
			InteractionPanelWidget->SetWidget(interactionPanel);
		}
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

void ARTS_BuildPreview::HandleCancelBuild()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
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
	_durabilityCurrent += 1.f;
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
	InitBuilding();
	GetWorld()->GetTimerManager().SetTimer(_buildTimer, this, &ThisClass::UpdateBuildingProgress, GetWorld()->GetDeltaSeconds(), true, 0);
}

void ARTS_BuildPreview::Select()
{
	InteractionPanelWidget->SetVisibility(true);
}
void ARTS_BuildPreview::Deselect()
{
	InteractionPanelWidget->SetVisibility(false);
}

void ARTS_BuildPreview::EndBuild()
{
	GetWorld()->GetTimerManager().ClearTimer(_buildTimer);
	OnBuildCompleted.Broadcast(this);
}

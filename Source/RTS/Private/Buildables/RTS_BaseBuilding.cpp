#include "Buildables/RTS_BaseBuilding.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Buildables/Building_InteractionPanel.h"
#include "UI/Generic/G_TextBlock.h"

ARTS_BaseBuilding::ARTS_BaseBuilding()
{
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	BuildingMesh->SetupAttachment(RootComponent);

	// Probably replace 3d widget with a UI menu widget in the future
	BuildingInteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BuildingInteractionWidgetComponent"));
	BuildingInteractionWidget->SetupAttachment(BuildingMesh);
	BuildingInteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	BuildingInteractionWidget->SetDrawAtDesiredSize(true);
}

void ARTS_BaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	BuildingInteractionWidget->SetVisibility(false);
}

void ARTS_BaseBuilding::Init(const FBuildingData& BuildData)
{
	_buildData = BuildData;
	// Initialize building mesh
	if (UStaticMesh* mesh = _buildData.BuildingMesh_Complete.LoadSynchronous())
		BuildingMesh->SetStaticMesh(mesh);

	// initialize 3d interaction panel widget
	if (TSubclassOf<UInteractionPanelWidget> interactionPanelClass = (InteractionPanelWidgetClass.LoadSynchronous()))
	{
		if (UBuilding_InteractionPanel* interactionPanel = CreateWidget<UBuilding_InteractionPanel>(UGameplayStatics::GetPlayerController(GetWorld(), 0), interactionPanelClass))
		{
			interactionPanel->Title->SetText(BuildData.Title);
			interactionPanel->OnDestroyButtonClicked.BindUObject(this, &ThisClass::HandleDestroy);
			BuildingInteractionWidget->SetWidget(interactionPanel);
		}
	}
}

void ARTS_BaseBuilding::HandleDestroy()
{
	Destroy();
}

void ARTS_BaseBuilding::LoadObjectData(FArchive& Ar)
{
	Init(_buildData);
}

void ARTS_BaseBuilding::Select()
{
	BuildingInteractionWidget->SetVisibility(true);
}

void ARTS_BaseBuilding::Deselect()
{
	BuildingInteractionWidget->SetVisibility(false);
}

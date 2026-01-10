#include "Buildables/RTS_BaseBuilding.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ARTS_BaseBuilding::ARTS_BaseBuilding()
{
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	BuildingMesh->SetupAttachment(RootComponent);
}

void ARTS_BaseBuilding::BeginPlay()
{
	Super::BeginPlay();
}

void ARTS_BaseBuilding::Init(const FBuildingData& BuildData)
{
	if (UStaticMesh* mesh = BuildData.BuildingMesh_Complete.LoadSynchronous())
		BuildingMesh->SetStaticMesh(mesh);
}

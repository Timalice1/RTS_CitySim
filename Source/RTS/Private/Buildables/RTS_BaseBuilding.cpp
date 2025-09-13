#include "Buildables/RTS_BaseBuilding.h"
#include "Buildables/BuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ARTS_BaseBuilding::ARTS_BaseBuilding()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollider;

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
	{
		BuildingMesh->SetStaticMesh(mesh);

		// Init collider size
		FVector Min, Max;
		BuildingMesh->GetLocalBounds(Min, Max);
		BoxCollider->SetBoxExtent(Max);
	}
}

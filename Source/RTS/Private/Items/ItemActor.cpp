#include "Items/ItemActor.h"
#include "Components/SphereComponent.h"
#include "Managers/RecoursesManagerSubsystem.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	RootComponent = ItemCollider;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ItemMesh->SetupAttachment(RootComponent);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	verify((_recoursesManager = GetWorld()->GetSubsystem<URecoursesManagerSubsystem>()));
	_recoursesManager->RegisterItem(this);
}

void AItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	_recoursesManager->UnRegisterItem(this);
	Super::EndPlay(EndPlayReason);
}

void AItemActor::PickUp()
{
	Destroy();
}

bool AItemActor::TryReserve()
{
	if (bReserved)
		return false;
	bReserved = true;
	return true;
}

#include "Items/ItemActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	RootComponent = ItemCollider;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ItemMesh->SetupAttachment(RootComponent);
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

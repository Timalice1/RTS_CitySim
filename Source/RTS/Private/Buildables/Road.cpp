#include "Buildables/Road.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ARoad::ARoad()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
}

void ARoad::BeginPlay()
{
	Super::BeginPlay();
}

void ARoad::SetRoadMesh(class UStaticMesh* InMesh)
{
	ISM->SetStaticMesh(InMesh);
}

void ARoad::AddInstance(const FVector& Location, const FRotator& Rotation)
{
	if (!ISM)
		return;
	ISM->AddInstance(FTransform(Rotation, Location), true);
}

void ARoad::SaveObjectData(FArchive& Ar)
{
	TArray<FTransform> instances;
	for (int32 i = 0; i < ISM->GetInstanceCount(); ++i)
		ISM->GetInstanceTransform(i, instances.Add_GetRef(FTransform()), true);
	Ar << instances;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%s - %d instances saved"), *GetName(), instances.Num()));
#endif
}

void ARoad::LoadObjectData(FArchive& Ar)
{
	TArray<FTransform> instances;
	Ar << instances;

	ISM->ClearInstances();
	for (FTransform &transform : instances)
		AddInstance(transform.GetLocation(), transform.Rotator());
}

ARoad_Preview::ARoad_Preview()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	Preview_ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	Preview_ISM->NumCustomDataFloats = 1; // It's required for updating material data value 
}

void ARoad_Preview::AddPreviewInstance(const FVector& Location, const FRotator& Rotation, bool IsValid)
{
	const int32 newInst = Preview_ISM->AddInstance(FTransform(
		                                               Rotation,
		                                               Location + FVector::UpVector * 1.f, // Move up slightly to be over existing road tiles
		                                               FVector(.9f, .9f, .9f)), true);
	Preview_ISM->SetCustomDataValue(newInst, 0, IsValid ? 1.f : 0.f, true);
}

void ARoad_Preview::ClearPreview()
{
	Preview_ISM->ClearInstances();
}

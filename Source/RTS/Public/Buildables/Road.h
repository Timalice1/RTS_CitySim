#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveGame/SaveableInterface.h"
#include "Road.generated.h"

UCLASS(Abstract)
class RTS_API ARoad : public AActor, public ISaveableInterface
{
	GENERATED_BODY()

public:
	ARoad();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UInstancedStaticMeshComponent> ISM = nullptr;

public:
	virtual void SetRoadMesh(class UStaticMesh* InMesh);
	virtual void AddInstance(const FVector& Location, const FRotator& Rotation);

	virtual void SaveObjectData_Implementation() override;
	virtual void LoadObjectData_Implementation() override;
};


UCLASS(Abstract)
class ARoad_Preview : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UInstancedStaticMeshComponent> Preview_ISM = nullptr;
	
public:
	ARoad_Preview();
	virtual void AddPreviewInstance(const FVector& Location, const FRotator& Rotation, bool IsValid);
	virtual void ClearPreview();
};

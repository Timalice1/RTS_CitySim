#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BaseBuilding.generated.h"

/**
 * Actual building class that will be used after building process was finished.
 * Here is actual build logic and behavior
 */
UCLASS()
class ARTS_BaseBuilding : public AActor, public ISaveableInterface
{
	GENERATED_BODY()

public:
	ARTS_BaseBuilding();
	virtual void BeginPlay() override;

	// Initialize building mesh and collider
	UFUNCTION(BlueprintCallable, Category = BaseBuilding)
	virtual void Init(const struct FBuildingData& BuildData);

	virtual void SaveObjectData_Implementation() override
	{
	}

	virtual void LoadObjectData_Implementation() override
	{
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> BuildingMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UBoxComponent> BoxCollider = nullptr;
};

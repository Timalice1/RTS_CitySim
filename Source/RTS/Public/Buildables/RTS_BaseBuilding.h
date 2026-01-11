#pragma once

#include "CoreMinimal.h"
#include "BuildingData.h"
#include "Core/Interfaces/SelectableInterface.h"
#include "GameFramework/Actor.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BaseBuilding.generated.h"

/**
 * Actual building class that will be used after building process was finished.
 * Here is actual build logic and behavior
 */
UCLASS()
class ARTS_BaseBuilding : public AActor, public ISaveableInterface, public ISelectableInterface
{
	GENERATED_BODY()

public:
	ARTS_BaseBuilding();
	virtual void BeginPlay() override;

	// Initialize building mesh and collider
	UFUNCTION(BlueprintCallable, Category = BaseBuilding)
	virtual void Init(const struct FBuildingData& BuildData);
	UFUNCTION(BlueprintCallable, Category = BaseBuilding)
	virtual void HandleDestroy();
	
	//~ Begin saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End saveable interface
	
	//~ Begin selectable interface
	virtual void Select() override;
	virtual void Deselect() override;
	//~ End selectable interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> BuildingMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UWidgetComponent> BuildingInteractionWidget = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widgets)
	TSoftClassPtr<class UBuilding_InteractionPanel> InteractionPanelWidgetClass;
		
	UPROPERTY(SaveGame)
	FBuildingData _buildData;
};

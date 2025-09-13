#pragma once

#include "CoreMinimal.h"
#include "BuildingData.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FBuildingData : public FTableRowBase
{
	GENERATED_BODY()

	/// Display title of this building*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = UI)
	FText Title = FText::FromString(TEXT("building_title"));

	/// Display icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = UI)
	TSoftObjectPtr<class UTexture2D> UI_Icon;

	/// Actual functional building class to be spawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = Config)
	TSoftClassPtr<class ARTS_BaseBuilding> Building_Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = Meshes)
	TSoftObjectPtr<class UStaticMesh> BuildingMesh_Complete; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = Meshes)
	TArray<TSoftObjectPtr<class UStaticMesh>> BuildingMesh_Stages;

	// Cell size of the world grid. Set during building initialization
	UPROPERTY(SaveGame)
	uint32 CellSize = 100.f;
	
	//================================================================================
	// Here can be other building data, such as building time, required resources etc.
	//================================================================================
};

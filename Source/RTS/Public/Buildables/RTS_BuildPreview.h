#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildables/BuildingData.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BuildPreview.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildFinishedEvent, class ARTS_BuildPreview*, PreviewActor);

UCLASS(Abstract)
class RTS_API ARTS_BuildPreview : public AActor, public ISaveableInterface
{
	GENERATED_BODY()

public:
	ARTS_BuildPreview();

	/// Init build data, building mesh and collider
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual void InitBuilding(const struct FBuildingData& BuildingData);

	/// Deploy and start build process
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual void StartBuild();

	/// Returns build data for this building 
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual FBuildingData GetBuildingData() const { return _buildingData; }

	/// Check if building is placeable. Updates material and IsPlaceable status
	/// By default, checks overlaps with other actors on the scene.
	/// For additional validations, override this function in child classes.
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual void ValidatePlacement();

	/// Returns placement status flag for this building.
	/// Placement status updates by ValidatePlacement()
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual bool IsPlaceable() const { return bIsPlaceable; }

	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}

	virtual void LoadObjectData(FArchive& Ar) override {}
	//~ End saveable interface

private: // Internal
	// Finish building process
	virtual void EndBuild();
	// Updates build process - update mesh and progress
	virtual void UpdateBuildingProgress();

protected: // Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> BuildingMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UBoxComponent> BoxCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	TSoftObjectPtr<class UMaterialInstance> OverlayPreviewMaterial;

public: // Events

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildFinishedEvent OnBuildCompleted;

private: // Internal data
	UPROPERTY()
	class UMaterialInstanceDynamic* _overlayMaterial = nullptr;

	FTimerHandle _buildTimer;

	UPROPERTY(SaveGame)
	float _buildProgress = 0.f;
	float _buildRate = 1.f;
	FBuildingData _buildingData;
	bool bIsPlaceable = true;
};

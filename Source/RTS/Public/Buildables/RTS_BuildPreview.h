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

	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual FVector GetBuildingBounds() const { return buildingBounds; }

	virtual void SetIsPlaceable(bool bIsPlaceable);
	
	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override;
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End saveable interface

private: // Internal
	// Finish building process
	virtual void EndBuild();
	// Updates build process - update mesh and progress
	virtual void UpdateBuildingProgress();

	virtual void InitMeshAndCollider();
	virtual void ApplyOverlay();

protected: // Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> BuildingMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	TSoftObjectPtr<class UMaterialInstance> OverlayPreviewMaterial;

public: // Events

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildFinishedEvent OnBuildCompleted;

private: // Internal data
	UPROPERTY()
	class UMaterialInstanceDynamic* _overlayMaterial = nullptr;

	UPROPERTY(SaveGame)
	FTimerHandle _buildTimer;

	UPROPERTY(SaveGame)
	float _buildProgress = 0.f;

	UPROPERTY(SaveGame)
	FBuildingData _buildingData;

	UPROPERTY(SaveGame)
	float _durabilityCurrent = 0.f;

	FVector buildingBounds;
};

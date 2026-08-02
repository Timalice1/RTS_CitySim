#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildables/BuildingData.h"
#include "Core/Interfaces/SelectableInterface.h"
#include "SaveGame/SaveableInterface.h"
#include "ConstructionProject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildFinishedEvent, class AConstructionProject*, PreviewActor);

UCLASS(Abstract)
class RTS_API AConstructionProject : public AActor,
                                     public ISaveableInterface,
                                     public ISelectableInterface
{
	GENERATED_BODY()

public:
	AConstructionProject();

	virtual void BeginPlay() override;

	/// Init build data, building mesh and collider
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual void Init(const struct FBuildingData& BuildingData);
	/// Deploy and start build process
	UFUNCTION(BlueprintCallable, Category = Buildable)
	virtual void StartBuild();

	UFUNCTION()
	virtual void HandleCancelBuild();

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

	//~ Begin selectable interface
	virtual void Select() override;
	virtual void Deselect() override;
	//~ End selectable interface

private: // Internal

	UFUNCTION()
	virtual void Handle_TaskFinished(class UTask* Task, bool bSuccess, const FString& message);

	// Updates build process - update mesh and progress
	virtual void UpdateBuildingProgress(const float& InProgress);

	virtual void InitBuilding();

	virtual void ApplyOverlay();
	
	virtual void EvaluateState() const;

protected: // Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> BuildingMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Comoponents)
	TObjectPtr<class UWidgetComponent> InteractionPanelWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	TSoftObjectPtr<class UMaterialInstance> OverlayPreviewMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	TSoftClassPtr<class UInteractionPanelWidget> InteractionPanelWidgetClass;

public: // Events

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildFinishedEvent OnBuildCompleted;

private: // Internal data
	UPROPERTY()
	class UMaterialInstanceDynamic* _overlayMaterial = nullptr;

	UPROPERTY(SaveGame)
	FBuildingData _buildingData;

	UPROPERTY(SaveGame)
	FGuid _buildingTask;

	FVector buildingBounds;

	UPROPERTY()
	TMap<TSubclassOf<AActor>, uint8> _deliveredResources;

	UFUNCTION()
	virtual void Handle_ResourceDelivered(class UTask* Task, bool bSuccess, const FString& Message);
};

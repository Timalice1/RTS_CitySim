#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveGame/SaveableInterface.h"
#include "BuilderComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBuilderComponent, Log, All);

UCLASS(ClassGroup = (Buildable), meta = (BlueprintSpawnableComponent))
class RTS_API UBuilderComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	UBuilderComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	/** Enable Building mode */
	UFUNCTION(BlueprintCallable, Category = BuilderComponent)
	virtual void EnterBuildMode(const struct FBuildingData& BuildingData);

	/// Enable road building mode
	// TODO: Add road data, mesh, etc
	UFUNCTION(BlueprintCallable, Category = "BuilderComponent|Roads")
	virtual void EnterRoadBuildMode();

	/// Rotates preview building in given direction
	virtual void RotateBuilding(const int32 direction = 1);

	/// Deploy preview building in target location, run building process
	UFUNCTION(BlueprintCallable, Category = BuilderComponent)
	virtual void BuildDeploy();

	/// Exits from a building mode (any), destroys current preview actor
	UFUNCTION(BlueprintCallable, Category = BuilderComponent)
	virtual void ExitBuildMode();

	/// Check if building mode is currently enabled
	UFUNCTION(BlueprintCallable, Category = BuilderComponent)
	bool IsBuildModeEnabled() const { return bIsBuildMode || bRoadBuildMode; }

	/**
	 * Snap given world location to virtual grid
	 * Returns snapped location
	 */
	UFUNCTION(BlueprintCallable, Category = "BuilderComponent|Grid")
	FVector SnapToGrid(const FVector& WorldLocation) const;

	/// Build completed callback handle
	UFUNCTION()
	virtual void HandleBuildCompleted(class ARTS_BuildPreview* PreviewActor);

	//======================Roads building=====================

	// Cache road draw start position
	virtual void Road_StartBuild();
	// Calculate placement for each road tile along bounding box sides
	virtual void Road_Build();
	// Commit road tiles on calculated places
	virtual void Road_Deploy();
	// Cancel road placement, remove current road preview
	virtual void Road_BuildCancel();

	struct FRoadCell
	{
		FVector Location;
		FRotator Rotation;
		bool bIsValid;

		FRoadCell(const FVector& InLocation, const FRotator& InRotation, bool IsValid)
			: Location(InLocation), Rotation(InRotation), bIsValid(IsValid) {}
	};

	//======================Events=============================
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildModeEnterEvent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildModeExitEvent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildCompletedEvent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoadBuildModeEnterEvent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoadBuildModeExitEvent);

	/// Called when component inter in building mode
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildModeEnterEvent OnBuildModeEnter;

	/// Called when building mode deactivated
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildModeExitEvent OnBuildModeExit;

	/// Called when any building process was finished
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildCompletedEvent OnBuildCompleted;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnRoadBuildModeEnterEvent OnRoadBuildEnter;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnRoadBuildModeExitEvent OnRoadBuildExit;

protected:
	bool bIsBuildMode = false;
	bool bRoadBuildMode = false;
	bool bPlaceRoad = false;

	/// Preview building class reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BuildingPreview)
	TSoftClassPtr<class ARTS_BuildPreview> BuildPreview_Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Road)
	TSoftClassPtr<class ARoad> Road_Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Road)
	TSoftClassPtr<class ARoad_Preview> RoadPreview_Class;

	/// Preview building drag interpolation speed
	UPROPERTY(EditDefaultsOnly, Category = BuildingPreview)
	float PreviewBuildDragInterpSpeed = 15.f;

	/// Preview building rotation interpolation speed
	/// TODO: rotation interpolation can be replaced with a curves
	UPROPERTY(EditDefaultsOnly, Category = BuildingPreview)
	float PreviewBuildRotationInterpSpeed = 15.f;

	/// Preview building height above the landscape/terrain
	UPROPERTY(EditDefaultsOnly, Category = BuildingPreview)
	float PreviewBuildFloatHeight = 100.f;

	/// Preview rotation step amount
	UPROPERTY(EditDefaultsOnly, Category = BuildingPreview,
		meta = (ForceUnits = "Degrees"))
	float BuildingRotationStep = 90.f;

	/// Cell size of the virtual grid
	UPROPERTY(EditDefaultsOnly, Category = Grid, meta = (ForceUnits = "Centimeters"))
	uint32 CellSize = 100;

	bool ValidatePlacement(const FVector& InLocation, const FVector& InObjectSize, AActor* IgnoredActor = 0) const;

private:
	UPROPERTY()
	class ARTS_PlayerController* player = nullptr;
	UPROPERTY()
	class UWorld* WorldContext = nullptr;
	UPROPERTY()
	class ARTS_BuildPreview* BuildPreviewActor = nullptr;
	FRotator TargetRotation = FRotator::ZeroRotator;

	/** Calculate and validate target transform for each road point */
	virtual void CreateRoadPreviewTiles(const FVector& Start, const FVector& Direction, const int32 Size);
	
	/** Return false if not a single tile of rad is valid*/
	virtual bool IsRoadValid();
public:
	//~ Begin saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End Saveable interface

private:
	//========= Road ==============================
	// Point, from which road started drawing
	FVector Road_StartPosition = FVector::ZeroVector;
	FVector primaryAxisDirection = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<class ARoad_Preview> RoadPreviewActor = nullptr;

	// Array of road tiles locations
	TArray<FRoadCell> RoadPoints;
};

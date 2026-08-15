#pragma once

#include "CoreMinimal.h"
#include "Core/Interfaces/InventoryInterface.h"
#include "GameFramework/Pawn.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BaseUnit.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUnit, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitBecomeAvailableEvent, class ARTS_BaseUnit*);

UCLASS()
class RTS_API ARTS_BaseUnit : public APawn,
                              public ISaveableInterface,
                              public IInventoryInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class USkeletalMeshComponent> BodyMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UFloatingPawnMovement> PawnMovement = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	TSoftObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

public:
	ARTS_BaseUnit();
	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;

	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End Saveable interface

	/* Assign a new task for that unit */
	virtual void AssignTask(class UTask* InTask);
	virtual bool HadActiveTask() const { return _currentTask != nullptr; }

	/* Load and returns default Behavior tree asset for that unit*/
	virtual class UBehaviorTree* GetDefaultBTAsset() const;

	virtual bool IsPointReachable(const FVector& InTarget) const;

	/// Currently called when unit finish his current task
	FOnUnitBecomeAvailableEvent OnUnitBecomeAvailable;

private:
	UFUNCTION()
	void Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message);

	// Updates current task object reference in blackboard
	virtual void UpdateBlackboardTask();

	UPROPERTY()
	TObjectPtr<class AAIController> _controller;

	UPROPERTY(SaveGame)
	FGuid _currentTaskID;

	UPROPERTY()
	class UTask* _currentTask = nullptr;

	//============== Inventory =====================
	UPROPERTY(SaveGame)
	TMap<TSubclassOf<AActor>, uint8> _inventory;
	UPROPERTY(SaveGame)
	uint8 _currentItemsCount = 0;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	uint8 MaxInventoryCapacity = UINT8_MAX;

public:
	UFUNCTION()
	virtual bool TryAddItem(const TSubclassOf<class AActor>& InItem) override;
	UFUNCTION()
	virtual uint8 GetAvailableCapacity() const override { return MaxInventoryCapacity - _currentItemsCount; }

	UFUNCTION()
	virtual uint8 GetItem(const TSubclassOf<class AActor>& InItem) const override;
	UFUNCTION()
	virtual void RemoveItem(const TSubclassOf<class AActor>& InItem, const uint8& Amount) override;
};

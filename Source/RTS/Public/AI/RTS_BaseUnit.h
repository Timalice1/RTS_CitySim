#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BaseUnit.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitBecomeAvailableEvent, const class ARTS_BaseUnit*)

UCLASS()
class RTS_API ARTS_BaseUnit : public APawn,
                              public ISaveableInterface
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
	virtual void BeginPlay() override;

	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End Saveable interface

	/* Assign a new task for that unit */
	virtual void AssignTask(class UTask* InTask);
	virtual bool HadActiveTask() const { return _currentTask != nullptr; }

	/// Loads and returns a unit-specific behavior tree reference
	virtual class UBehaviorTree* GetBTAsset() const;

public:
	/// Currently called when unit finish his current task
	FOnUnitBecomeAvailableEvent OnUnitBecomeAvailable;

private:
	void Handle_TaskCompleted(UTask* Task);

	// Updates current task object reference in blackboard
	virtual void UpdateBlackboardTask();

private:
	UPROPERTY(SaveGame)
	FGuid _currentTaskID;

	UPROPERTY()
	class UTask* _currentTask = nullptr;
};

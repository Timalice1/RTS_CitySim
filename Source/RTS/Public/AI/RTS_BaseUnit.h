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

public:
	ARTS_BaseUnit();
	virtual void BeginPlay() override;

	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {}
	virtual void LoadObjectData(FArchive& Ar) override;
	//~ End Saveable interface

	virtual bool HadActiveTask() const { return _currentTask != nullptr; }

	virtual void AssignTask(class UTask* InTask);

	FOnUnitBecomeAvailableEvent OnUnitBecomeAvailable;
	
	virtual class UBehaviorTree* GetBTAsset() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = AI)
	TSoftObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

private:
	UPROPERTY(SaveGame)
	FGuid _currentTaskID;

	UPROPERTY()
	class UTask* _currentTask = nullptr;

	void Handle_TaskCompleted(UTask* Task);

	virtual void UpdateBlackboardTask();
};

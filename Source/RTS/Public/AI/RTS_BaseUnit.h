#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BaseUnit.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUnit, Log, All);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UInventoryComponent> InventoryComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	TSoftObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(BlueprintReadWrite, Category = Unit)
	TObjectPtr<class UUnitsManagerSubsystem> _unitsManager = nullptr;

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

	UPROPERTY(EditDefaultsOnly, Category = Unit)
	float IdleNotificationInterval = 3.f;

protected:
	virtual void EnterIdleState();
	virtual void ExitIdleState();

private:
	UFUNCTION()
	void Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message);

	UPROPERTY()
	TObjectPtr<class AAIController> _controller;

	UPROPERTY(SaveGame)
	FGuid _currentTaskID;

	UPROPERTY()
	class UTask* _currentTask = nullptr;

	UPROPERTY()
	FTimerHandle _idleNotificationTimer;
};

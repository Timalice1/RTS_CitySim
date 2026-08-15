#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "SaveGame/SaveableInterface.h"
#include "UObject/Object.h"
#include "Task.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTaskFinishedEvent, class UTask*, Task, bool, bSuccess, const FString&, message);

UCLASS(BlueprintType, Blueprintable)
class RTS_API UTask : public UObject, public ISaveableInterface
{
	GENERATED_BODY()

public:
	/* Unique task ID, used for access after serialization and deserialization */
	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn))
	FGuid TaskID;

	/* 
	 * Displayable name of that task
	 * Will be shown in a unit infobox
	 */
	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn), Category = Task)
	FName ActionName;

	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn), Category = Task)
	FGameplayTag TaskCategory;

	/* Target location of that task*/
	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn), Category = Task)
	FVector TaskLocation;

	/// Max number of workers that can be assigned
	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn), Category = Task)
	uint8 MaxJobSlots = 1;

public:
	/// Fires when the task has been completed / canceled / failed / interrupted
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Events)
	FOnTaskFinishedEvent OnTaskFinished;

public:
	/* Main enter point into task performing */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Task)
	void Execute(class AAIController* Executor);
	virtual void Execute_Implementation(class AAIController* Executor);

	/// Check if possible to assign that task to anyone. Overridable per-task.
	UFUNCTION(BlueprintCallable, Category = Task)
	virtual bool TryAssign(const class ARTS_BaseUnit* InUnit);
	
	/// Check if it's possible to take that task - e.g., task are already taken by someone else
	UFUNCTION(BlueprintCallable, Category = Task)
	virtual bool IsLocked() const { return _bLocked; }

	UFUNCTION(BlueprintCallable, Category = Task)
	virtual class AAIController* GetExecutor() const { return _executor; }
	
	UFUNCTION()
	virtual void ReserveJobSlot();
	
private:
	UPROPERTY(SaveGame)
	uint8 _reservedSlots = 0;

	UPROPERTY(SaveGame)
	bool _bLocked = false;

	UPROPERTY(SaveGame)
	TObjectPtr<class AAIController> _executor;

protected:
	virtual UWorld* GetWorld() const override;

public:
	virtual void SaveObjectData(FArchive& Ar) override {};
	virtual void LoadObjectData(FArchive& Ar) override {};
};

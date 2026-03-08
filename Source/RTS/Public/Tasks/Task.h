#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Task.generated.h"

/// Task entity structure
/// 
/// Holds data, info and requirements for a specific task
/// TODO: Make this entity data-table editable (optional)
USTRUCT(BlueprintType, Blueprintable)
struct FTaskEntity
{
	GENERATED_BODY()

	/* Task name */
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = Task, meta = (ExposeOnSpawn))
	FName TaskName;

	/* Task target position in the world (if any outer actor) */
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = Task)
	FVector TaskLocation;

	/* Max work points */
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = Task)
	float MaxWork;

	/*Max required worker slots for this task*/
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = Task)
	int RequiredWorkers;

	// TODO: Add a task type Gameplay tag or enum

	FTaskEntity() {}

	FTaskEntity(const FName InTaskName,
	            const FVector& InTaskLocation = FVector::ZeroVector,
	            const float InWorkAmount = 100.f,
	            const int InWorkSlots = 1)
		: TaskName(InTaskName),
		  TaskLocation(InTaskLocation),
		  MaxWork(InWorkAmount),
		  RequiredWorkers(InWorkSlots) {};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTaskCompletedEvent, class UTask*);
DECLARE_DELEGATE_OneParam(FOnProgressChangedEvent, const float);

UCLASS(BlueprintType, Blueprintable)
class RTS_API UTask : public UObject
{
	GENERATED_BODY()

public:
	/* Unique task ID for accessing after serialization and deserialization */
	UPROPERTY(BlueprintReadOnly, SaveGame, meta = (ExposeOnSpawn))
	FGuid TaskID;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Task, meta = (ExposeOnSpawn))
	FTaskEntity TaskEntity;

public:
	/// Fires when the task is completed
	FOnTaskCompletedEvent OnTaskCompletedEvent;

	/// Fires when task progress is updated
	FOnProgressChangedEvent OnProgressChanged;

public:
	UFUNCTION(BlueprintCallable, Category = Task)
	virtual void RunTask();

	/* Set a new priority value for that task*/
	UFUNCTION(BlueprintCallable, Category = "Task|Priority")
	virtual void SetTaskPriority(const int InPriority);

	UFUNCTION(BlueprintCallable, Category = "Task|Priority")
	virtual int GetTaskPriority() const { return _taskPriority; }

	/* Reserves a slot for a unit on that task*/
	virtual void ReserveWorkerSlot()
	{
		if (_reservedSlots < TaskEntity.RequiredWorkers)
			_reservedSlots++;
	}

	virtual int GetReservedSlots() const { return _reservedSlots; }

private:
	UPROPERTY(SaveGame)
	float _workCurrent = 0.f;

	UPROPERTY(SaveGame)
	float _taskProgress = 0.f;

	UPROPERTY(SaveGame)
	int _taskPriority = 5;

	UPROPERTY(SaveGame)
	int _reservedSlots = 0;

private:
	virtual UWorld* GetWorld() const override;
};

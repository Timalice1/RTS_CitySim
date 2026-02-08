#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Task.generated.h"

UENUM()
enum ETaskStatus
{
	Active,
	Pending
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTaskCompletedEvent, const class UTask*);
DECLARE_DELEGATE_OneParam(FOnProgressChangedEvent, const float);

UCLASS()
class RTS_API UTask : public UObject
{
	GENERATED_BODY()

public:
	/**/
	UPROPERTY(SaveGame)
	FGuid TaskID;

	/* Task name (probably will be changed with ID)*/
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Task)
	FName TaskName = FName("task_name");

	/* Task target position in the world*/
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Task)
	FVector TaskLocation = FVector::ZeroVector;

	/*Max work points*/
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Task)
	float MaxWork = 100.f;

	/// Fires when the task is completed
	FOnTaskCompletedEvent OnTaskCompletedEvent;

	/// Fires when task progress is changed in any way
	FOnProgressChangedEvent OnProgressChanged;

	virtual void RunTask();

	virtual void SetTaskStatus(const TEnumAsByte<ETaskStatus> NewStatus) { _taskStatus = NewStatus; }
	virtual TEnumAsByte<ETaskStatus> GetTaskStatus() const { return _taskStatus; }

private:
	UPROPERTY(SaveGame)
	float WorkCurrent = 0.f;

	UPROPERTY(SaveGame)
	float TaskProgress = 0.f;

	TEnumAsByte<ETaskStatus> _taskStatus;

private:
	virtual UWorld* GetWorld() const override;
	virtual void UpdateTaskProgress();
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Task.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTaskCompletedEvent);
DECLARE_DELEGATE_OneParam(FOnProgressChangedEvent, const float);

UCLASS()
class RTS_API UTask : public UObject
{
	GENERATED_BODY()

public:
	/* Task name (probably will be changed with ID)*/
	UPROPERTY(BlueprintReadOnly, Category = Task)
	FName TaskName = FName("task_name");

	/* Task target position in the world*/
	UPROPERTY(BlueprintReadOnly, Category = Task)
	FVector TaskLocation = FVector::ZeroVector;

	/*Max work points*/
	UPROPERTY(BlueprintReadOnly, Category = Task)
	float MaxWork = 100.f;

	/// Fires when the task is completed
	FOnTaskCompletedEvent OnTaskCompletedEvent; 

	/// Fires when task progress is changed in any way
	FOnProgressChangedEvent OnProgressChanged;

	virtual UWorld* GetWorld() const override;
	virtual void RunTask();

private:
	float WorkCurrent = 0.f;
	float TaskProgress = 0.f;
	virtual void UpdateTaskProgress();
};

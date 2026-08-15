#pragma once

#include "CoreMinimal.h"
#include "Task.h"
#include "ProgressiveTask.generated.h"

DECLARE_DELEGATE_OneParam(FOnProgressChangedEvent, const float);

/**
 * Derived task object to be used for a tasks with a progression behavior
 */
UCLASS()
class RTS_API UProgressiveTask : public UTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = ProgressiveTask)
	float WorkAmount = 100.f;

protected:
	UFUNCTION()
	virtual void Execute_Implementation(class AAIController* Executor) override;

public:
	FOnProgressChangedEvent OnProgressChanged;

	UFUNCTION(BlueprintCallable, Category = ProgressiveTask)
	virtual void UpdateTaskProgress();

private:
	float _workCurrent = 0.f;
	float _taskProgress = 0.f;
	FTimerHandle _updateTimer;
};

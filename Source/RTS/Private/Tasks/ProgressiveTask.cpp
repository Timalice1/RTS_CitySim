#include "Tasks/ProgressiveTask.h"

void UProgressiveTask::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);
	GetWorld()->GetTimerManager().SetTimer(_updateTimer, this, &UProgressiveTask::UpdateTaskProgress, .01f, true, 0);
}

void UProgressiveTask::UpdateTaskProgress()
{
	if (WorkAmount <= 0)
		return;

	_workCurrent += 10.f * GetWorld()->GetDeltaSeconds();
	_taskProgress = _workCurrent / WorkAmount;
	OnProgressChanged.ExecuteIfBound(_taskProgress);
	if (_taskProgress >= 1)
		OnTaskFinished.Broadcast(this, true, FString("TaskCompleted"));
}

#include "Tasks/ProgressiveTask.h"
#include "Kismet/KismetSystemLibrary.h"

void UProgressiveTask::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);
	UpdateTaskProgress();
}

void UProgressiveTask::UpdateTaskProgress()
{
	if (WorkAmount <= 0)
		return;

	_workCurrent += 10.f * GetWorld()->GetDeltaSeconds();
	_taskProgress = _workCurrent / WorkAmount;
	OnProgressChanged.ExecuteIfBound(_taskProgress);

#if WITH_EDITOR
	UKismetSystemLibrary::DrawDebugString(GetWorld(),
	                                      TaskLocation,
	                                      FString::Printf(TEXT("%.2f"), _taskProgress), 0, FLinearColor::Blue, .01f);
#endif

	if (_taskProgress >= 1)
		OnTaskFinished.Broadcast(this, true, FString("TaskCompleted"));
}

#include "Tasks/Task.h"

#include "Kismet/KismetSystemLibrary.h"

UWorld* UTask::GetWorld() const
{
	if (GIsEditor)
		return GWorld;
	return GetOuter()->GetWorld();
}

void UTask::RunTask()
{
	if (TaskEntity.MaxWork == 0)
		return;

	// TODO: Instead of static increment, make each assigned worker apply own amount of progress points,
	// TODO: based on workers skills, level, etc
	// float progressAmount = TaskDifficulty * UnitSkill * UnitLevel ...
	// _workCurrent += progressAmount * GetWorld()->GetDeltaSeconds();
	_workCurrent += 10.f * GetWorld()->GetDeltaSeconds();
	_taskProgress = _workCurrent / TaskEntity.MaxWork;

	OnProgressChanged.ExecuteIfBound(_taskProgress);

	UKismetSystemLibrary::DrawDebugString(GetWorld(),
	                                      TaskEntity.TaskLocation,
	                                      FString::Printf(TEXT("%.2f"), _taskProgress), 0, FLinearColor::Blue, .01f);

	// Notify that task has been completed
	if (_taskProgress >= 1)
		OnTaskCompletedEvent.Broadcast(this);
}

void UTask::SetTaskPriority(const int InPriority)
{
	if (InPriority == _taskPriority)
		return;
	_taskPriority = InPriority;
}

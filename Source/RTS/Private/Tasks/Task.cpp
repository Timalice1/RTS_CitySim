#include "Tasks/Task.h"

UWorld* UTask::GetWorld() const
{
	if (GIsEditor)
	{
		return GWorld;
	}
	return GetOuter()->GetWorld();
}

void UTask::RunTask()
{
	FTimerHandle _progressTimer;
	GetWorld()->GetTimerManager().SetTimer(_progressTimer, this, &ThisClass::UpdateTaskProgress, .01f, true, 0);
}

void UTask::UpdateTaskProgress(/*const float InProgressPointsAmount*/)
{
	if (MaxWork == 0)
		return;

	// TODO: Instead of static increment, make each assigned worker apply own amount of progress points,
	// Based on workers skills, level, etc
	WorkCurrent += 2.f * GetWorld()->GetDeltaSeconds();
	TaskProgress = WorkCurrent / MaxWork;

	OnProgressChanged.ExecuteIfBound(TaskProgress);
}

#include "Managers/TasksManagerSubsystem.h"
#include "Tasks/Task.h"

void UTasksManagerSubsystem::AddTask(const UTask* InTask)
{
	if (!InTask)
		return;

	// Broadcast a delegate about new task was created
	OnTaskCreated.Execute(InTask);
}

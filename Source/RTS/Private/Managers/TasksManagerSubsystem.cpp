#include "Managers/TasksManagerSubsystem.h"

#include "AI/RTS_BaseUnit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "Tasks/Task.h"

void UTasksManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	verify((_saveGameManager = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>()))
	_saveGameManager->OnSaveGameRequested.AddDynamic(this, &ThisClass::Handle_SaveRequested);
	_saveGameManager->OnSaveLoaded.AddDynamic(this, &ThisClass::Handle_SaveLoaded);
}


void UTasksManagerSubsystem::RegisterTask(UTask* InTask)
{
	if (!IsValid(InTask) || Tasks.Contains(InTask))
		return;

	InTask->OnTaskCompletedEvent.AddUObject(this, &ThisClass::Handle_TaskCompleted);
	Tasks.Add(InTask);

	OnNewTaskAvailable.Broadcast(InTask);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("%s task was registered"), *InTask->TaskEntity.TaskName.ToString()));
#endif
}

UTask* UTasksManagerSubsystem::GetTaskByID(const FGuid& InGuid) const
{
	return *Tasks.FindByPredicate([InGuid](const UTask* Task)
	{
		return Task->TaskID == InGuid;
	});
}

UTask* UTasksManagerSubsystem::RequestTaskForUnit(const class ARTS_BaseUnit* Unit)
{
	// Check unit state first
	if (!Unit || Unit->HadActiveTask())
		return nullptr;


	// Filter tasks by lack of workers
	const TArray<UTask*> availableTasks = Tasks.FilterByPredicate([](const UTask* task)
	{
		return task->GetReservedSlots() < task->TaskEntity.RequiredWorkers;
	});

	// Evaluate filtered tasks score
	UTask* bestTask = nullptr;
	if (!availableTasks.IsEmpty())
	{
		int bestScore = 0;
		for (UTask* task : availableTasks)
		{
			const float dist = FVector::DistSquared(Unit->GetActorLocation(), task->TaskEntity.TaskLocation);
			const float score = task->GetTaskPriority() * dist; // TODO: Probably add some weights for each scorer
			if (score > bestScore)
			{
				bestScore = score;
				bestTask = task;
			}
		}
	}

	return bestTask;
}

void UTasksManagerSubsystem::Handle_SaveRequested(class URTS_SaveGame* SaveGameObject)
{
	for (UTask* task : Tasks)
	{
		FTaskSaveDataRecord taskSaveRecord;
		taskSaveRecord.bytes = _saveGameManager->SerializeObject(task);
		SaveGameObject->TaskData.Add(taskSaveRecord);
	}
}

void UTasksManagerSubsystem::Handle_SaveLoaded(class URTS_SaveGame* SaveGameObject)
{
	// Clear current tasks
	Tasks.Empty();

	// Deserialize saved tasks data and register new tasks
	for (const FTaskSaveDataRecord& objectRecord : SaveGameObject->TaskData)
	{
		if (UTask* task = NewObject<UTask>())
		{
			_saveGameManager->DeserializeObject(task, objectRecord.bytes);
			RegisterTask(task);
		}
	}
}

void UTasksManagerSubsystem::Handle_TaskCompleted(UTask* Task)
{
	Tasks.Remove(Task);
}

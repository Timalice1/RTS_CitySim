#include "Managers/TasksManagerSubsystem.h"
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

FGuid UTasksManagerSubsystem::RegisterTask(UTask* InTask)
{
	if (!InTask)
		return FGuid();

	const FGuid taskID = FGuid::NewGuid();
	InTask->TaskID = taskID;
	Tasks.Add(InTask);

	// Broadcast a delegate about new task was created
	OnTaskCreated.Execute(InTask);
	return taskID;
}

UTask* UTasksManagerSubsystem::GetTaskByID(const FGuid& InGuid) const
{
	return *Tasks.FindByPredicate([InGuid](const UTask* Task)
	{
		return Task->TaskID == InGuid;
	});
}

void UTasksManagerSubsystem::Handle_SaveRequested(class URTS_SaveGame* SaveGameObject)
{
	for (UTask* task : Tasks)
	{
		FObjectSaveDataRecord taskSaveRecord;
		taskSaveRecord.bytes = _saveGameManager->SerializeObject(task);
		SaveGameObject->ObjectsData.Add(taskSaveRecord);
	}
}

void UTasksManagerSubsystem::Handle_SaveLoaded(class URTS_SaveGame* SaveGameObject)
{
	Tasks.Empty();

	for (const FObjectSaveDataRecord& objectRecord : SaveGameObject->ObjectsData)
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

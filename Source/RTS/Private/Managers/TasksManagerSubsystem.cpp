#include "Managers/TasksManagerSubsystem.h"

#include "AI/RTS_BaseUnit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "Tasks/Task.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY(LogTasksManager);

void UTasksManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	verify((_saveGameManager = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>()))
	_saveGameManager->OnSaveGameRequested.AddDynamic(this, &ThisClass::Handle_SaveRequested);
	_saveGameManager->OnSaveLoaded.AddDynamic(this, &ThisClass::Handle_SaveLoaded);
}


void UTasksManagerSubsystem::RegisterTask(UTask* InTask)
{
	if (!IsValid(InTask))
		return;

	InTask->OnTaskFinished.AddDynamic(this, &ThisClass::Handle_TaskFinished);
	Tasks.Add(InTask);

	UE_LOG(LogTasksManager, Display,
	       TEXT("Task %s:%s has been created and registered."),
	       *InTask->ActionName.ToString(),
	       *InTask->TaskID.ToString());

	OnNewTaskAvailable.Broadcast(InTask);
}

UTask* UTasksManagerSubsystem::GetTaskByID(const FGuid& InGuid) const
{
	return *Tasks.FindByPredicate([InGuid](const UTask* Task)
	{
		return Task->TaskID == InGuid;
	});
}

TArray<UTask*> UTasksManagerSubsystem::FindTasksByCategory(const FGameplayTag& InCategory) const
{
	if (Tasks.IsEmpty())
		return TArray<UTask*>();

	return Tasks.FilterByPredicate([InCategory](const UTask* Task)
	{
		return Task->TaskCategory == InCategory;
	});
}

UTask* UTasksManagerSubsystem::RequestTaskForUnit(const class ARTS_BaseUnit* Unit)
{
	// Check unit state first
	if (!Unit || Unit->HadActiveTask() || Tasks.IsEmpty())
		return nullptr;

	// Evaluate filtered tasks score
	UTask* bestTask = nullptr;
	float bestScore = 0;
	for (UTask* task : Tasks)
	{
		if (task->IsLocked())
			continue;

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(),
		                                      task->TaskLocation,
		                                      25.f, 12, FLinearColor::Yellow, 5.f, 2.f);

		// Score tasks based on their priority and distance to target unit
		const float dist = FVector::Dist(Unit->GetActorLocation(), task->TaskLocation); // Take nearest one task
		const float score = task->GetTaskPriority() / dist; // TODO: Probably add some weights for each scorer

		if (score > bestScore)
		{
			bestScore = score;
			bestTask = task;
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

void UTasksManagerSubsystem::Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message)
{
	Tasks.Remove(Task);
	Task->MarkAsGarbage();
}

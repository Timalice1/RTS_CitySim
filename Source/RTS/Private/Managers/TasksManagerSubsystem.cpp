#include "Managers/TasksManagerSubsystem.h"

#include "AI/RTS_BaseUnit.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "Tasks/Task.h"
#include "GameplayTagContainer.h"
#include "Managers/UnitsManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogTasksManager);

void UTasksManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	verify((_saveGameManager = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>()))
	_saveGameManager->OnSaveGameRequested.AddDynamic(this, &ThisClass::Handle_SaveRequested);
	_saveGameManager->OnSaveLoaded.AddDynamic(this, &ThisClass::Handle_SaveLoaded);

	GetWorld()->GetSubsystem<UUnitsManagerSubsystem>()->OnUnitBecomeAvailable.AddDynamic(
		this, &ThisClass::OnUnitAvailable);
}

void UTasksManagerSubsystem::RegisterTask(UTask* InTask)
{
	if (!IsValid(InTask))
		return;

	InTask->OnTaskFinished.AddDynamic(this, &ThisClass::Handle_TaskFinished);
	Tasks.Add(InTask);
	InTask->TaskStatus = AVAILABLE;
}

void UTasksManagerSubsystem::OnUnitAvailable(ARTS_BaseUnit* InUnit)
{
	if (!InUnit) return;

	if (UTask* task = RequestTaskForUnit(InUnit)) {
		InUnit->AssignTask(task);
		task->ReserveJobSlot();
	}
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
	if (!Unit || Tasks.IsEmpty()) {
		UE_LOG(LogTasksManager, Warning, TEXT("Unit are null or tasks array was empty"));
		return nullptr;
	}


	// Evaluate filtered tasks score
	UTask* bestTask = nullptr;
	float bestScore = 0;
	for (UTask* task : Tasks) {
		// Skip if task cannot be assigned/executed
		if (!task->IsAvailableFor(Unit)) {
			UE_LOG(LogTasksManager, Warning, TEXT("Task is not available for assignment and execution"));
			continue;
		}


		// TODO: Move the score evaluation to task object side
		// Score tasks based on their priority and distance to target unit
		const float dist = FVector::Dist(Unit->GetActorLocation(), task->TaskLocation); // Take nearest one task
		const float score = 1 / dist; // TODO: Probably add some weights for each scorer

		if (score > bestScore) {
			bestScore = score;
			bestTask = task;
		}
	}

	return bestTask;
}

void UTasksManagerSubsystem::Handle_SaveRequested(class URTS_SaveGame* SaveGameObject)
{
	for (UTask* task : Tasks) {
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
	for (const FTaskSaveDataRecord& objectRecord : SaveGameObject->TaskData) {
		if (UTask* task = NewObject<UTask>()) {
			_saveGameManager->DeserializeObject(task, objectRecord.bytes);
			RegisterTask(task);
		}
	}
}

void UTasksManagerSubsystem::Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(Task);
	Tasks.Remove(Task);
	if (bSuccess)
		Task->TaskStatus = COMPLETED;
}

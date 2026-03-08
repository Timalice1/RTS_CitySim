#include "Managers/UnitsManagerSubsystem.h"
#include "Managers/TasksManagerSubsystem.h"
#include "AI/RTS_BaseUnit.h"
#include "Tasks/Task.h"

void UUnitsManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->OnNewTaskAvailable.AddUObject(this, &ThisClass::Handle_TaskAvailable);
}

void UUnitsManagerSubsystem::RegisterUnit(ARTS_BaseUnit* InNewUnit)
{
	if (!InNewUnit)
		return;

	_registeredUnits.Add(InNewUnit);
	InNewUnit->OnUnitBecomeAvailable.AddUObject(this, &ThisClass::Handle_UnitAvailable);
}


void UUnitsManagerSubsystem::Handle_TaskAvailable(UTask* Task)
{
	// Find the best unit for that task

	ReAssign();
}

void UUnitsManagerSubsystem::Handle_UnitAvailable(const ARTS_BaseUnit* ARTS_BaseUnit)
{
	// Find the best task for that unit

	ReAssign();
}

void UUnitsManagerSubsystem::ReAssign()
{
	for (ARTS_BaseUnit* unit : _registeredUnits)
	{
		if (UTask* task = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->RequestTaskForUnit(unit))
		{
			unit->AssignTask(task);
			task->ReserveWorkerSlot();
		}
	}
}

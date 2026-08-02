#include "Managers/UnitsManagerSubsystem.h"
#include "Managers/TasksManagerSubsystem.h"
#include "AI/RTS_BaseUnit.h"
#include "Tasks/Task.h"

void UUnitsManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->OnNewTaskAvailable.AddUObject(this, &ThisClass::OnTaskAvailable);
}

void UUnitsManagerSubsystem::RegisterUnit(ARTS_BaseUnit* InNewUnit)
{
	if (!InNewUnit)
		return;

	_registeredUnits.Add(InNewUnit);
	InNewUnit->OnUnitBecomeAvailable.AddUObject(this, &ThisClass::OnUnitAvailable);
}

int UUnitsManagerSubsystem::GetAvailableUnitsNum() const
{
	return _registeredUnits.FilterByPredicate([](const ARTS_BaseUnit* Unit)
	{
		return !Unit->HadActiveTask();
	}).Num();
}


void UUnitsManagerSubsystem::OnTaskAvailable(UTask* Task)
{
	for (ARTS_BaseUnit* unit : _registeredUnits)
	{
		if (unit->HadActiveTask())
			continue;
		OnUnitAvailable(unit);
	}
}

void UUnitsManagerSubsystem::OnUnitAvailable(ARTS_BaseUnit* InUnit)
{
	if (UTask* task = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->RequestTaskForUnit(InUnit))
	{
		InUnit->AssignTask(task);
		task->ReserveJobSlot();
	}
}

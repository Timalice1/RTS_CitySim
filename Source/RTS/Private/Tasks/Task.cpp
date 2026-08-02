#include "Tasks/Task.h"
#include "AIController.h"
#include "AI/RTS_BaseUnit.h"

UWorld* UTask::GetWorld() const
{
	if (GIsEditor)
		return GWorld;
	return GetOuter()->GetWorld();
}

void UTask::Execute_Implementation(AAIController* Executor)
{
	if (Executor)
		_executor = Executor;
}

bool UTask::TryAssign(const ARTS_BaseUnit* InUnit)
{
	if (_bLocked || !InUnit || ! InUnit->IsPointReachable(TaskLocation))
		return false;
	return true;
}

void UTask::ReserveJobSlot()
{
	_reservedSlots++;
	if (_reservedSlots == MaxJobSlots)
		_bLocked = true;
}

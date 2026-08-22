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

bool UTask::IsAvailableFor(const ARTS_BaseUnit* InUnit)
{
	if (_bLocked || TaskStatus != AVAILABLE)
		return false;
	if (!InUnit || !InUnit->IsPointReachable(TaskLocation)) {
		UE_LOG(LogTemp, Warning, TEXT("IsAvailableFor - Unit invalid or target point was unreachable"))
		return false;
	}
	return true;
}

void UTask::ReserveJobSlot()
{
	_reservedSlots++;
	if (_reservedSlots == MaxJobSlots) {
		TaskStatus = CLAIMED;
		_bLocked = true;
	}
}

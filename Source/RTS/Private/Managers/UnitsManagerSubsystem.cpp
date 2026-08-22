#include "Managers/UnitsManagerSubsystem.h"
#include "AI/RTS_BaseUnit.h"

void UUnitsManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UUnitsManagerSubsystem::RegisterUnit(ARTS_BaseUnit* InNewUnit)
{
	if (!InNewUnit) return;
	_registeredUnits.Add(InNewUnit);
}

TSet<TObjectPtr<ARTS_BaseUnit>> UUnitsManagerSubsystem::GetAvailableUnits()
{
	return _idleUnits;
}

void UUnitsManagerSubsystem::NotifyUnitAvailable(ARTS_BaseUnit* InUnit)
{
	if (!InUnit) return;
	_idleUnits.Add(InUnit);
	OnUnitBecomeAvailable.Broadcast(InUnit);
}

void UUnitsManagerSubsystem::NotifyUnitUnavailable(ARTS_BaseUnit* InUnit)
{
	if (!InUnit || !_idleUnits.Contains(InUnit)) return;
	_idleUnits.Remove(InUnit);
}

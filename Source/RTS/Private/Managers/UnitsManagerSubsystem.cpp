#include "Managers/UnitsManagerSubsystem.h"

#include "AI/RTS_BasePawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/TasksManagerSubsystem.h"

void UUnitsManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	verify((TasksManager = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()) != nullptr);

	TasksManager->OnTaskCreated.BindDynamic(this, &ThisClass::AssignUnit);
}

void UUnitsManagerSubsystem::RegisterUnit(ARTS_BasePawn* InNewUnit)
{
	// Add new unit to the units list
	_registeredUnits.Add(InNewUnit);
}

void UUnitsManagerSubsystem::AssignUnit(const UTask* InTask)
{
	// Filter units by their status
	TArray<TObjectPtr<ARTS_BasePawn>> availableUnits = _registeredUnits.FilterByPredicate([](const ARTS_BasePawn* unit)
	{
		// Add here some additional checks, like if unit is alive, units profession, task priority etc.
		return unit->GetUnitStatus() == Idle;
	});

	// Check if there is any available units for that task
	if (availableUnits.Num() == 0)
	{
		// Add This task to backlog list in tasks manager

		// Notify if no units available
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Units not Assigned")));
#endif
		return;
	}
	
	// Sort available units by fitness (distance, skill, task priority etc.)
	
	// Select required amount of units by formula min(require, available)
	for (const ARTS_BasePawn* unit : availableUnits)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), unit->GetActorLocation(), 50.f, 12, FLinearColor::Green, 10.f, 1.f);

	// Change assigned units status
	
	// Assign this task for selected units
	
}

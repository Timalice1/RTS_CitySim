#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnitsManagerSubsystem.generated.h"

class ARTS_BasePawn;

UCLASS()
class RTS_API UUnitsManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/*Register new unit in the manager*/
	virtual void RegisterUnit(ARTS_BasePawn* InNewUnit);

private:
	/*Assign single or multiple units to the target task*/
	UFUNCTION()
	virtual void AssignUnit(const class UTask* InTask);

private:
	UPROPERTY()
	TObjectPtr<class UTasksManagerSubsystem> TasksManager = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<ARTS_BasePawn>> _registeredUnits;
};

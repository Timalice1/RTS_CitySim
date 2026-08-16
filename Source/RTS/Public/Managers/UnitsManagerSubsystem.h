#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnitsManagerSubsystem.generated.h"

class ARTS_BaseUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitBecomeAvailableEvent, ARTS_BaseUnit*, InUnit);

UCLASS()
class RTS_API UUnitsManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/*Register new unit in the manager*/
	virtual void RegisterUnit(ARTS_BaseUnit* InNewUnit);
	virtual TSet<TObjectPtr<ARTS_BaseUnit>> GetAvailableUnits();

	UFUNCTION(BlueprintCallable, Category = UnitsManager)
	virtual void NotifyUnitAvailable(ARTS_BaseUnit* InUnit);
	virtual void NotifyUnitUnavailable(ARTS_BaseUnit* InUnit);

	UPROPERTY()
	FOnUnitBecomeAvailableEvent OnUnitBecomeAvailable;

private:
	/*Registry of all units on the world*/
	UPROPERTY()
	TSet<TObjectPtr<ARTS_BaseUnit>> _registeredUnits;

	/*Registry of all idle and available for work units*/
	UPROPERTY()
	TSet<TObjectPtr<ARTS_BaseUnit>> _idleUnits;
};

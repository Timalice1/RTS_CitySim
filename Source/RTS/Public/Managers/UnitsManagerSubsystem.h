#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnitsManagerSubsystem.generated.h"

class ARTS_BaseUnit;

UCLASS()
class RTS_API UUnitsManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	// TODO: Keep separated lists for all units, available units, busy units, etc.
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/*Register new unit in the manager*/
	virtual void RegisterUnit(ARTS_BaseUnit* InNewUnit);
	
	/// Returns the number of units, that didn't have any tasks assigned
	UFUNCTION(BlueprintCallable, Category = UnitsManager)
	virtual int GetAvailableUnitsNum() const;

private: 
	
	// TODO: Add separated registry for available units
	UPROPERTY()
	TArray<TObjectPtr<ARTS_BaseUnit>> _registeredUnits;

	virtual void OnTaskAvailable(class UTask* Task);
	virtual void OnUnitAvailable(ARTS_BaseUnit* InUnit);
};

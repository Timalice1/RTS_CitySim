#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnitsManagerSubsystem.generated.h"

class ARTS_BaseUnit;

UCLASS()
class RTS_API UUnitsManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/*Register new unit in the manager*/
	virtual void RegisterUnit(ARTS_BaseUnit* InNewUnit);
	
	UFUNCTION(BlueprintCallable, Category = UnitsManager)
	virtual int GetAvailableUnitsNum() const;

private: 
	
	// TODO: Add separated registry for available units
	UPROPERTY()
	TArray<TObjectPtr<ARTS_BaseUnit>> _registeredUnits;

	virtual void OnTaskAvailable(class UTask* Task);
	virtual void OnUnitAvailable(ARTS_BaseUnit* InUnit);
};

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


private:
	UPROPERTY()
	TArray<TObjectPtr<ARTS_BaseUnit>> _registeredUnits;

	virtual void Handle_TaskAvailable(class UTask* Task);
	virtual void Handle_UnitAvailable(const ARTS_BaseUnit* ARTS_BaseUnit);

	virtual void ReAssign();
};

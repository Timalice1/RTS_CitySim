#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitsInterface.generated.h"

UINTERFACE()
class UUnitsInterface : public UInterface
{
	GENERATED_BODY()
};

UENUM()
enum EUnitStatus
{
	Idle,
	Work
};

class RTS_API IUnitsInterface
{
	GENERATED_BODY()

public:
	/*Get current unit status*/
	virtual TEnumAsByte<EUnitStatus> GetUnitStatus() const = 0;

	/*Change current unit status*/
	virtual void SetUnitStatus(const TEnumAsByte<EUnitStatus> InNewStatus) = 0;

	/// Assign a new task for this unit 
	virtual void SetCurrentTask(class UTask* InTask) = 0;
};

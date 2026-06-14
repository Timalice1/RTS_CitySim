#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

UINTERFACE()
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class RTS_API IInventoryInterface
{
	GENERATED_BODY()
	
public:
	virtual bool TryAddItem(const TSubclassOf<class AActor> &InItem) = 0;
};

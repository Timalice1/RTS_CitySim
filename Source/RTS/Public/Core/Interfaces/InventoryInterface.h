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
	virtual bool TryAddItem(const TSubclassOf<class AActor>& InItem) = 0;
	virtual uint8 GetAvailableCapacity() const = 0;
	/// Returns an amount of specific items in the inventory
	virtual uint8 GetItem(const TSubclassOf<class AActor> &InItem) const = 0;
	virtual void RemoveItem(const TSubclassOf<class AActor> &InItem, const uint8 &Amount) = 0;
};

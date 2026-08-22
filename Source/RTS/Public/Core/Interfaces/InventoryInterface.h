#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

UINTERFACE()
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class AItemActor;

class RTS_API IInventoryInterface
{
	GENERATED_BODY()

public:
	virtual bool TryAddItem(const TSubclassOf<AItemActor>& InItem) = 0;
	virtual uint8 GetAvailableCapacity() const = 0;
	/// Returns an amount of specific items in the inventory
	virtual uint8 GetItem(const TSubclassOf<AItemActor>& InItem) const = 0;
	virtual void RemoveItem(const TSubclassOf<AItemActor>& InItem, const uint8& Amount) = 0;
};

#include "Components/InventoryComponent.h"
#include "Items/ItemActor.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::TryAddItem(const TSubclassOf<AItemActor>& InItem)
{
	if (_currentItemsCount >= MaxCapacity)
		return false;

	_inventory.FindOrAdd(InItem)++;
	_currentItemsCount++;
	return true;
}

void UInventoryComponent::RemoveItem(const TSubclassOf<AItemActor>& InItem, const uint8& Amount)
{
	if (const uint8* current = _inventory.Find(InItem)) {
		if (*current == Amount)
			_inventory.Remove(InItem);
		else
			_inventory[InItem] = *current - Amount;
		_currentItemsCount -= Amount;
	}
}

uint8 UInventoryComponent::GetItem(const TSubclassOf<AItemActor>& InItem)
{
	if (const uint8* current = _inventory.Find(InItem))
		return *current;
	return 0;
}

uint8 UInventoryComponent::GetAvailableCapacity()
{
	return MaxCapacity - _currentItemsCount;
}

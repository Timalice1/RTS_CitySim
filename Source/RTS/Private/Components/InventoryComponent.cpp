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

int UInventoryComponent::RemoveItem(const TSubclassOf<AItemActor>& InItem, const int& Amount)
{
	int toRemove = 0;
	if (const int* current = _inventory.Find(InItem)) {
		toRemove = FMath::Min(*current, Amount);
		if (toRemove == *current)
			_inventory.Remove(InItem);
		else
			_inventory[InItem] -= toRemove;
		_currentItemsCount -= toRemove;
	}
	return toRemove;
}

int UInventoryComponent::GetItem(const TSubclassOf<AItemActor>& InItem)
{
	if (const int* current = _inventory.Find(InItem))
		return *current;
	return 0;
}

int UInventoryComponent::GetAvailableCapacity()
{
	return MaxCapacity - _currentItemsCount;
}

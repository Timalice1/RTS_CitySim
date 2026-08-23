#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AItemActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION()
	virtual bool TryAddItem(const TSubclassOf<AItemActor>& InItem);

	UFUNCTION()
	virtual int RemoveItem(const TSubclassOf<AItemActor>& InItem, const int& Amount);

	UFUNCTION()
	virtual int GetItem(const TSubclassOf<AItemActor>& InItem);

	UFUNCTION()
	virtual int GetAvailableCapacity();

	UFUNCTION()
	virtual TMap<TSubclassOf<AItemActor>, int> GetInventory() { return _inventory; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InventoryComponent)
	int MaxCapacity = 100;

	/*
	 * TODO: For a storage inventory
	 *  allowed items list
	 *  reserved items list  (if two units gonna need the same item)
	 *  ReserveItems(ItemType, Amount)
	 *  ReleaseReserved
	 *  GetAvailable -> Num of non-reserved items 
	 *  CanAcceptItem? -> check for allowed items list
	 */

private:
	UPROPERTY(SaveGame)
	int _currentItemsCount = 0;

	UPROPERTY(SaveGame)
	TMap<TSubclassOf<AItemActor>, int> _inventory;
};

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
	virtual void RemoveItem(const TSubclassOf<AItemActor>& InItem, const uint8& Amount);

	UFUNCTION()
	virtual uint8 GetItem(const TSubclassOf<AItemActor>& InItem);

	UFUNCTION()
	virtual uint8 GetAvailableCapacity();

	UFUNCTION()
	virtual TMap<TSubclassOf<AItemActor>, uint8> GetInventory() { return _inventory; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InventoryComponent)
	uint8 MaxCapacity = UINT8_MAX;

private:
	UPROPERTY(SaveGame)
	uint8 _currentItemsCount = 0;

	UPROPERTY(SaveGame)
	TMap<TSubclassOf<AItemActor>, uint8> _inventory;
};

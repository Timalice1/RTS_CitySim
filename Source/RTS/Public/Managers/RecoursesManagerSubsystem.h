#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RecoursesManagerSubsystem.generated.h"

class UInventoryComponent;
class AItemActor;

// Probably replace with a UObject instead
USTRUCT()
struct FItemSearchResult
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AItemActor> ItemActor = nullptr;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Storage = nullptr;

	int Quantity = 0; // 1 for a ItemActor, GetAvailable() for a storage
	int Requested = 0; // Actual amount to withdraw

	FVector GetLocation() const;

	// void ReserveItems();
	// void InteractWithSource();
};

/**
 * Recourses registry and efficient management
 */
UCLASS()
class RTS_API URecoursesManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	TSet<TObjectPtr<UInventoryComponent>> _storagesRegistry;
	TMultiMap<TSubclassOf<AItemActor>, TObjectPtr<AItemActor>> _itemsRegistry;

public:
	UFUNCTION()
	virtual void RegisterItem(AItemActor* InItemActor);

	UFUNCTION()
	virtual void UnRegisterItem(AItemActor* InItemActor);

	UFUNCTION()
	virtual void RegisterStorage(UInventoryComponent* InStorage);

	UFUNCTION()
	virtual void UnRegisterStorage(UInventoryComponent* InStorage);

	UFUNCTION()
	virtual TArray<FItemSearchResult> FindItemsByType(const TSubclassOf<AItemActor>& InItemType);
};

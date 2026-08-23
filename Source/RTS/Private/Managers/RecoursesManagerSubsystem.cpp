#include "Managers/RecoursesManagerSubsystem.h"
#include "Items//ItemActor.h"
#include "Components/InventoryComponent.h"


void URecoursesManagerSubsystem::RegisterItem(AItemActor* InItemActor)
{
	if (!IsValid(InItemActor)) return;
	_itemsRegistry.Add(InItemActor->GetClass(), InItemActor);
}

void URecoursesManagerSubsystem::UnRegisterItem(AItemActor* InItemActor)
{
	if (!IsValid(InItemActor)) return;
	_itemsRegistry.RemoveSingle(InItemActor->GetClass(), InItemActor);
}

void URecoursesManagerSubsystem::RegisterStorage(UInventoryComponent* InStorage)
{
	if (!IsValid(InStorage)) return;
	_storagesRegistry.Add(InStorage);
}

void URecoursesManagerSubsystem::UnRegisterStorage(UInventoryComponent* InStorage)
{
	if (!IsValid(InStorage)) return;
	_storagesRegistry.Remove(InStorage);
}

TArray<FItemSearchResult> URecoursesManagerSubsystem::FindItemsByType(const TSubclassOf<AItemActor>& InItemType)
{
	TArray<FItemSearchResult> result;

	TArray<TObjectPtr<AItemActor>> outItems;
	_itemsRegistry.MultiFind(InItemType, outItems);
	for (AItemActor* item : outItems) {
		if (IsValid(item) && !item->IsReserved())
			result.Add({item, nullptr, 1});
	}

	return result;
}

FVector FItemSearchResult::GetLocation() const
{
	if (ItemActor)
		return ItemActor->GetActorLocation();
	if (Storage)
		return Storage->GetOwner()->GetActorLocation(); // Use an entrance proxy point location
	return FVector::ZeroVector;
}

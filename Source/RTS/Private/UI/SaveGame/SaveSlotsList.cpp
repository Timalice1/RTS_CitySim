#include "UI/SaveGame/SaveSlotsList.h"
#include "Components/ListView.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "UI/SaveGame/SaveSlotEntry.h"

void USaveSlotsList::NativeConstruct()
{
	Super::NativeConstruct();
	SlotsList->OnItemSelectionChanged().AddUObject(this, &ThisClass::Handle_SelectionChanged);
	GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->OnGameSaved.AddDynamic(this, &ThisClass::UpdateList);
}

void USaveSlotsList::UpdateList()
{
	TArray<UObject*> items;

	for (const FString& savingSlot : GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->LoadSavingsSlots())
	{
		if (USlotEntryMetadata* slotEntry = NewObject<USlotEntryMetadata>())
		{
			slotEntry->SlotName = savingSlot;
			items.Insert(slotEntry, 0);
		}
	}
	SlotsList->SetListItems(items);
	SlotsList->ClearSelection();
	OnListUpdated.Broadcast();
}

void USaveSlotsList::Handle_SelectionChanged(UObject* Object)
{
	if (Object)
		OnSlotSelected.Broadcast(Object);
}

void USaveSlotsList::RemoveSlot(const FString& InSlotName) {}

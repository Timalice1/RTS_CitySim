#include "UI/SaveGame/SaveSlotsList.h"
#include "Components/ListView.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "UI/SaveGame/SaveSlotEntry.h"

void USaveSlotsList::NativeConstruct()
{
	Super::NativeConstruct();
	SlotsList->OnItemSelectionChanged().AddUObject(this, &ThisClass::Handle_SelectionChanged);
}

void USaveSlotsList::UpdateList()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("TEXT")));
#endif
	SlotsList->ClearListItems();
	for (const FString& savingSlot : GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->LoadSavingsSlots())
		AddSaveSlot(savingSlot);
}

void USaveSlotsList::Handle_SelectionChanged(UObject* Object)
{
	if (Object)
		OnSlotSelected.Broadcast(Object);
}

void USaveSlotsList::AddSaveSlot(const FString& InSlotName)
{
	if (USlotEntryMetadata* slotEntry = NewObject<USlotEntryMetadata>())
	{
		slotEntry->SlotName = InSlotName;
		SlotsList->AddItem(slotEntry);
	}
}

void USaveSlotsList::RemoveSlot(const FString& InSlotName) {}

#include "UI/SaveGame/LoadGameMenu.h"

#include "SaveGame/SaveGameSubsystem.h"
#include "UI/Generic/G_Button.h"
#include "UI/SaveGame/SaveSlotEntry.h"
#include "UI/SaveGame/SaveSlotsList.h"

void ULoadGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	DeleteSave_Button->OnClicked.AddDynamic(this, &ThisClass::Handle_DeleteSave);
	LoadGame_Button->OnClicked.AddDynamic(this, &ThisClass::Handle_LoadGame);

	SlotsList->OnSlotSelected.AddDynamic(this, &ThisClass::Handle_SelectionChanged);
	SlotsList->OnListUpdated.AddDynamic(this, &ThisClass::Handle_ListUpdated);
}

void ULoadGameMenu::Handle_DeleteSave()
{
	if (SelectedSlotData)
		GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->DeleteSaveSlot(SelectedSlotData->SlotName);
	SlotsList->UpdateList();
}

void ULoadGameMenu::Handle_LoadGame()
{
	if (SelectedSlotData)
		GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->Request_LoadGame(SelectedSlotData->SlotName);
}

void ULoadGameMenu::Handle_SelectionChanged(UObject* SelectedItem)
{
	if (USlotEntryMetadata* slotData = Cast<USlotEntryMetadata>(SelectedItem))
		SelectedSlotData = slotData;

	LoadGame_Button->SetIsEnabled(true);
	DeleteSave_Button->SetIsEnabled(true);
}

void ULoadGameMenu::Handle_ListUpdated()
{
	LoadGame_Button->SetIsEnabled(false);
	DeleteSave_Button->SetIsEnabled(false);
}

void ULoadGameMenu::Handle_VisibilityChanged(ESlateVisibility InVisibility)
{
	Super::Handle_VisibilityChanged(InVisibility);
	LoadGame_Button->SetIsEnabled(false);
	DeleteSave_Button->SetIsEnabled(false);
}

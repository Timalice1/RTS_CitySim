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

	LoadGame_Button->SetIsEnabled(false);
	DeleteSave_Button->SetIsEnabled(false);

	SlotsList->OnSlotSelected.AddDynamic(this, &ThisClass::Handle_SelectionChanged);
}

void ULoadGameMenu::Handle_DeleteSave() {}

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

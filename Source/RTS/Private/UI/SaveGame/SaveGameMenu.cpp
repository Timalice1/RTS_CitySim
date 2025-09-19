#include "UI/SaveGame/SaveGameMenu.h"

#include "Components/EditableText.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "UI/Generic/G_Button.h"
#include "UI/SaveGame/SaveSlotEntry.h"
#include "UI/SaveGame/SaveSlotsList.h"

void USaveGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	SaveGame_Button->OnClicked.AddDynamic(this, &ThisClass::Handle_SaveButtonClick);
	SlotsList->OnSlotSelected.AddDynamic(this, &ThisClass::Handle_SlotSelected);
}

void USaveGameMenu::Handle_SaveButtonClick()
{
	if (const FString slotName = SlotName_Input->GetText().ToString(); !slotName.IsEmpty())
		GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->Request_SaveGame(slotName);
}

void USaveGameMenu::Handle_SlotSelected(UObject* SelectedItem)
{
	if (USlotEntryMetadata* slotData = Cast<USlotEntryMetadata>(SelectedItem))
		SlotName_Input->SetText(FText::FromString(slotData->SlotName));
}

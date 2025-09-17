#include "UI/SaveGame/SaveGameMenu.h"

#include "Components/EditableText.h"
#include "GameFramework/GameUserSettings.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "UI/Generic/G_Button.h"
#include "UI/SaveGame/SaveSlotsList.h"

void USaveGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	SaveGame_Button->OnClicked.AddDynamic(this, &ThisClass::USaveGameMenu::Handle_SaveButtonClick);
}

void USaveGameMenu::Handle_SaveButtonClick()
{
	const FString slotName = SlotName_Input->GetText().ToString();
	GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->Request_SaveGame(slotName);
	SlotsList->AddSaveSlot(slotName);
}
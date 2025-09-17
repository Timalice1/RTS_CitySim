#include "UI/SaveGame/LoadGameMenu.h"
#include "UI/Generic/G_Button.h"

void ULoadGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	DeleteSave_Button->OnClicked.AddDynamic(this, &ThisClass::Handle_DeleteSave);
	LoadGame_Button->OnClicked.AddDynamic(this, &ThisClass::Handle_LoadGame);
}

void ULoadGameMenu::Handle_DeleteSave() {}
void ULoadGameMenu::Handle_LoadGame() {}

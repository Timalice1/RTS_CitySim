#include "UI/SaveGame/G_SaveLoad_Panel.h"

#include "UI/Generic/G_TextBlock.h"
#include "UI/SaveGame/SaveSlotsList.h"

void UG_SaveLoad_Panel::NativeConstruct()
{
	Super::NativeConstruct();
	OnVisibilityChanged.AddDynamic(this, &ThisClass::UG_SaveLoad_Panel::Handle_VisibilityChanged);
}

void UG_SaveLoad_Panel::NativePreConstruct()
{
	Super::NativePreConstruct();
	PanelTitle->SetText(PanelTitleText);
}

void UG_SaveLoad_Panel::Handle_VisibilityChanged(ESlateVisibility InVisibility)
{
	SlotsList->UpdateList();
}

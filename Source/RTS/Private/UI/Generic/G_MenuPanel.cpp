#include "UI/Generic/G_MenuPanel.h"
#include "UI/Generic/G_TextBlock.h"

void UG_MenuPanel::NativePreConstruct()
{
	Super::NativePreConstruct();
	MenuTitle->SetText(Title);
}

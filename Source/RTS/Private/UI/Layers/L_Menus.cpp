#include "UI/Layers/L_Menus.h"
#include "Components/WidgetSwitcher.h"

void UL_Menus::NativeConstruct()
{
	Super::NativeConstruct();
	MenusSwitcher->SetActiveWidget(nullptr);
}

void UL_Menus::ToggleMenu(UUserWidget* InMenuWidget)
{
	if (!InMenuWidget)
		return;
	
	if (ActiveMenu)
	{
		ActiveMenu->SetVisibility(ESlateVisibility::Collapsed);
		if (ActiveMenu == InMenuWidget)
		{
			MenusSwitcher->SetActiveWidget(nullptr);
			ActiveMenu = nullptr;
			return;
		}
	}
	
	ActiveMenu = InMenuWidget;
	MenusSwitcher->SetActiveWidget(ActiveMenu);
	ActiveMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

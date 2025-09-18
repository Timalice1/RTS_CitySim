#include "UI/Layers/L_Menus.h"
#include "Components/WidgetSwitcher.h"

void UL_Menus::NativeConstruct()
{
	Super::NativeConstruct();
	MenusSwitcher->SetActiveWidget(nullptr);
	
	for (UWidget* panel : MenusSwitcher->GetAllChildren())
		panel->SetVisibility(ESlateVisibility::Collapsed);
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

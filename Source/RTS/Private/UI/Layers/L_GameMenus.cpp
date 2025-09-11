#include "UI/Layers/L_GameMenus.h"

#include "Buildables/BuilderComponent.h"
#include "Core/RTS_PlayerController.h"
#include "UI/Generic/G_MenuPanel.h"

void UL_GameMenus::NativeConstruct()
{
	Super::NativeConstruct();
	verify((player = Cast<ARTS_PlayerController>(GetOwningPlayer())) != nullptr);

	// ENSURE ALL PANELS WIDGETS ARE COLLAPSED INITIALLY
	
	if (BuildingsMenuPanel) // Since widget is optional
	{
		BuildingsMenuPanel->SetVisibility(ESlateVisibility::Collapsed);
		BuildingsMenuPanel->OnVisibilityChanged.AddDynamic(this, &ThisClass::HandleBuildMenuVisibilityChanged);
	}
}

bool UL_GameMenus::CloseActivePanel()
{
	if (!ActivePanelWidget)
		return false;
	
	ActivePanelWidget->SetVisibility(ESlateVisibility::Collapsed);
	ActivePanelWidget = nullptr;
	return true;
}

void UL_GameMenus::ToggleBuildingsPanel()
{
	TogglePanel(BuildingsMenuPanel);
}

void UL_GameMenus::HandleBuildMenuVisibilityChanged(ESlateVisibility InVisibility)
{
	if (UBuilderComponent* builder = player->FindComponentByClass<UBuilderComponent>())
	{
		if (InVisibility != ESlateVisibility::Collapsed)
			builder->ExitBuildMode();
	}
}

void UL_GameMenus::TogglePanel(UUserWidget* TargetWidget)
{
	if (!TargetWidget)
		return;

	if (TargetWidget == ActivePanelWidget || TargetWidget->IsVisible())
	{
		TargetWidget->SetVisibility(ESlateVisibility::Collapsed);
		ActivePanelWidget = nullptr;
		return;
	}

	ActivePanelWidget = TargetWidget;
	ActivePanelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

#include "UI/RTS_HUD.h"
#include "GameFramework/Actor.h"
#include "UI/Layers/L_GameMenus.h"
#include "UI/Layers/L_Modals.h"
#include "UI/Layers/L_Menus.h"

DEFINE_LOG_CATEGORY(LogHUD);

void ARTS_HUD::BeginPlay()
{
	Super::BeginPlay();
	verify((owningPlayer = GetOwningPlayerController()) != nullptr);

	owningPlayer->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
	InitLayers();
}

void ARTS_HUD::InitLayers()
{
	verify((GameMenusLayer_Widget = CreateWidget<UL_GameMenus>(owningPlayer, GameMenusLayer_Class.LoadSynchronous())) != nullptr);
	verify((MenusLayer_Widget = CreateWidget<UL_Menus>(owningPlayer, MenusLayer_Class.LoadSynchronous())) != nullptr);
	verify((ModalsLayer_Widget = CreateWidget<UL_Modals>(owningPlayer, ModalsLayer_Class.LoadSynchronous())) != nullptr);
	UILayers.Add(ModalsLayer_Widget);
	UILayers.Add(GameMenusLayer_Widget);
	UILayers.Add(MenusLayer_Widget);

	for (UUserWidget* layer : UILayers)
		layer->AddToViewport();
}

void ARTS_HUD::ToggleBuildingsPanel()
{
	GameMenusLayer_Widget->ToggleBuildingsPanel();
}

bool ARTS_HUD::CloseActivePanel()
{
	return GameMenusLayer_Widget->CloseActivePanel();
}

void ARTS_HUD::TogglePauseMenu()
{
	bPauseMenuOpened = !bPauseMenuOpened;
	MenusLayer_Widget->SetVisibility(bPauseMenuOpened ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	for (UUserWidget* layer : UILayers)
	{
		if (layer != MenusLayer_Widget)
			layer->SetVisibility(bPauseMenuOpened ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	}
}

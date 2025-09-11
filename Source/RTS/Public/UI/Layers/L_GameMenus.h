#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_GameMenus.generated.h"

class UG_MenuPanel;
class ARTS_PlayerController;

/**
 * Game menus layer widget
 * Holds and manage all in-game menus widgets
 * Such as inventory panel, buildings panel etc
 */
UCLASS(Abstract)
class RTS_API UL_GameMenus : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = GameMenusLayer)
	virtual bool CloseActivePanel();

	UFUNCTION(BlueprintCallable, Category = GameMenusLayer)
	virtual void ToggleBuildingsPanel();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UG_MenuPanel> BuildingsMenuPanel = nullptr;

private:
	UFUNCTION()
	virtual void HandleBuildMenuVisibilityChanged(ESlateVisibility InVisibility);

	UFUNCTION()
	virtual void TogglePanel(UUserWidget* TargetWidget);

	UPROPERTY()
	ARTS_PlayerController* player = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> ActivePanelWidget = nullptr;
};

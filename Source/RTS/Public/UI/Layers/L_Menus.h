#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_Menus.generated.h"

/**
 * Menus layer widget
 * Used for pause menu, settings panel, save/load panel, exit game etc.
 */
UCLASS(Abstract)
class RTS_API UL_Menus : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> MenusSwitcher = nullptr;

	UFUNCTION(BlueprintCallable, Category = MenusLayer)
	virtual void ToggleMenu(UUserWidget* InMenuWidget);

private:
	UPROPERTY()
	UUserWidget* ActiveMenu = nullptr;
};

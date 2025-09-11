#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_MenuPanel.generated.h"

UCLASS(Abstract)
class RTS_API UG_MenuPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> MenuTitle = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> MenuBorder = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNamedSlot> MenuBody = nullptr;

	UPROPERTY(EditInstanceOnly, Category = MenuPanelWidget)
	FText Title = FText::FromString(TEXT("menu_title"));
};

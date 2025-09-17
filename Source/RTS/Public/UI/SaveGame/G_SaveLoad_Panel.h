#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_SaveLoad_Panel.generated.h"

UCLASS()
class RTS_API UG_SaveLoad_Panel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> PanelTitle = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USaveSlotsList> SlotsList = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Config)
	FText PanelTitleText = FText::FromString(TEXT("panel_title"));

private:
	UFUNCTION()
	virtual void Handle_VisibilityChanged(ESlateVisibility InVisibility);
};

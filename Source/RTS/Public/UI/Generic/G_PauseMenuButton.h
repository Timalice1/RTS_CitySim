#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_PauseMenuButton.generated.h"

UCLASS(Abstract)
class RTS_API UG_PauseMenuButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuButtonClickedEvent);

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnPauseMenuButtonClickedEvent OnPauseMenuButtonClicked;

protected:
	UPROPERTY(EditInstanceOnly, Category = PauseMenuButton)
	FText ButtonLabelText = FText::FromString(TEXT("pause_menu_button"));

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> ButtonLabel;

	UFUNCTION()
	void HandleButtonClicked();
};

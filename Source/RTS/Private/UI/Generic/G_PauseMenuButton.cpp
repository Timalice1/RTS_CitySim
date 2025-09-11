#include "UI/Generic/G_PauseMenuButton.h"

#include "Components/Button.h"
#include "UI/Generic/G_TextBlock.h"

void UG_PauseMenuButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	ButtonLabel->SetText(ButtonLabelText);	
}

void UG_PauseMenuButton::NativeConstruct()
{
	Super::NativeConstruct();
	MenuButton->OnClicked.AddDynamic(this, &ThisClass::HandleButtonClicked);
}

void UG_PauseMenuButton::HandleButtonClicked()
{
	OnPauseMenuButtonClicked.Broadcast();
}

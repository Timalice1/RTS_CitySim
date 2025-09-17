#include "UI/Generic/G_Button.h"

#include "Components/Button.h"
#include "UI/Generic/G_TextBlock.h"

void UG_Button::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &ThisClass::Handle_Clicked);
}

void UG_Button::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ButtonLabel)
		ButtonLabel->SetText(ButtonLabelText);
}

void UG_Button::Handle_Clicked()
{
	OnClicked.Broadcast();
}

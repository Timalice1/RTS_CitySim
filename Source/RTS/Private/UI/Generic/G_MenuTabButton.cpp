#include "UI/Generic/G_MenuTabButton.h"

#include "Components/Button.h"
#include "Components/Image.h"

void UG_MenuTabButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UTexture2D* texture = IconTexture.LoadSynchronous())
		TabIcon->SetBrushFromTexture(texture);
}

void UG_MenuTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabButton)
		TabButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UG_MenuTabButton::HandleClicked()
{
	OnTabButtonClicked.Broadcast(this);
}

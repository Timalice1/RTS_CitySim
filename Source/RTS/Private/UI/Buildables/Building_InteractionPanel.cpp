#include "UI/Buildables/Building_InteractionPanel.h"

#include "Components/Button.h"

void UBuilding_InteractionPanel::NativeConstruct()
{
	Super::NativeConstruct();
	if (DestroyButton)
		DestroyButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UBuilding_InteractionPanel::HandleClicked()
{
	OnDestroyButtonClicked.Execute();
}

#include "UI/Buildables/BuildItemWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Generic/G_TextBlock.h"

void UBuildItemWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UBuildItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemButton->OnClicked.AddDynamic(this, &ThisClass::HandleButtonClicked);
}

void UBuildItemWidget::Init(const FBuildingData& BuildData)
{
	_buildData = BuildData;

	ItemTitle->SetText(_buildData.Title);
	if (UTexture2D* texture = _buildData.UI_Icon.LoadSynchronous())
		ItemIcon->SetBrushFromTexture(texture);
}

void UBuildItemWidget::HandleButtonClicked()
{
	OnBuildItemSelected.Broadcast(_buildData);
}

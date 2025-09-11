#include "UI/Buildables/G_Button_BuildMenuOptional.h"

#include "Buildables/BuilderComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UG_Button_BuildMenuOptional::NativePreConstruct()
{
	Super::NativePreConstruct();
	ButtonIcon->SetBrushFromTexture(IconTexture.LoadSynchronous());
	SizeBox->SetHeightOverride(Size.X);
	SizeBox->SetWidthOverride(Size.Y);
}

void UG_Button_BuildMenuOptional::NativeConstruct()
{
	Super::NativeConstruct();
	verify((BuilderComponent = Cast<UBuilderComponent>(GetOwningPlayer()->GetComponentByClass(UBuilderComponent::StaticClass()))));
	Button->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UG_Button_BuildMenuOptional::HandleClicked()
{
	OnClicked.Broadcast();
}

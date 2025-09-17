#include "UI/SaveGame/SaveSlotsList.h"

#include "Components/ScrollBox.h"
#include "SaveGame/SaveGameSubsystem.h"
#include "UI/Generic/G_TextBlock.h"

void USaveSlotsList::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateList();
}

void USaveSlotsList::AddSaveSlot(const FString& InSlotName)
{
	if (!slotNameTextTemplate)
		return;

	if (UG_TextBlock* slotNameTextBlock = CreateWidget<UG_TextBlock>(this, slotNameTextTemplate.LoadSynchronous()))
	{
		TArray<UWidget*> tempArray = Slots_Container->GetAllChildren();
		slotNameTextBlock->SetText(FText::FromString(InSlotName));
		tempArray.Insert(slotNameTextBlock, 0);
		Slots_Container->ClearChildren();

		for (UWidget* widget : tempArray)
			Slots_Container->AddChild(widget);
	}
}

void USaveSlotsList::RemoveSlot(const FString& InSlotName) {}

void USaveSlotsList::UpdateList()
{
	Slots_Container->ClearChildren();
	if (const TArray<FString> savings = GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->LoadSavingsSlots(); !savings.IsEmpty())
	{
		for (const FString& savingSlot : savings)
			AddSaveSlot(savingSlot);
	}
}
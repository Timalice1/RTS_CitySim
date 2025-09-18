#include "UI/SaveGame/SaveSlotEntry.h"

#include "Components/Border.h"
#include "UI/Generic/G_TextBlock.h"

void USaveSlotEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

void USaveSlotEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	if (USlotEntryMetadata* slotData = Cast<USlotEntryMetadata>(ListItemObject))
	{
		_name = slotData->SlotName;
		SlotName->SetText(FText::FromString(slotData->SlotName));
	}
	slotBorder->SetBrushColor(FLinearColor::Transparent);
}

void USaveSlotEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
	slotBorder->SetBrushColor(bIsSelected ? FLinearColor::Gray : FLinearColor::Transparent);
}

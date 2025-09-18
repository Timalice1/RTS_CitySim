#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotEntry.generated.h"

UCLASS()
class RTS_API USlotEntryMetadata : public UObject
{
	GENERATED_BODY()

public:
	FString SlotName = TEXT("slot_name");
};

UCLASS(Abstract)
class RTS_API USaveSlotEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

	FString _name;
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> slotBorder = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> SlotName = nullptr;

	//~ IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	//~ IUserObjectListEntry
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotsList.generated.h"

UCLASS(Abstract)
class RTS_API USaveSlotsList : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> Slots_Container;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	TSoftClassPtr<class UG_TextBlock> slotNameTextTemplate;

public:
	virtual void NativeConstruct() override;

	virtual void AddSaveSlot(const FString& InSlotName);
	virtual void RemoveSlot(const FString& InSlotName);

	virtual void UpdateList();
};

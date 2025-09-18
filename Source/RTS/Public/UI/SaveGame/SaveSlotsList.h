#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotsList.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveSlotSelectedDelegate, UObject*, SelectedItem);

UCLASS(Abstract)
class RTS_API USaveSlotsList : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> SlotsList;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	TSoftClassPtr<class UG_TextBlock> slotNameTextTemplate;

private:
	void Handle_SelectionChanged(UObject* Object);
	virtual void AddSaveSlot(const FString& InSlotName);

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void UpdateList();

	virtual void RemoveSlot(const FString& InSlotName);

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnSaveSlotSelectedDelegate OnSlotSelected;
};

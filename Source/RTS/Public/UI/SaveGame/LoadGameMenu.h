#pragma once

#include "CoreMinimal.h"
#include "G_SaveLoad_Panel.h"
#include "LoadGameMenu.generated.h"

UCLASS()
class RTS_API ULoadGameMenu : public UG_SaveLoad_Panel
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_Button> LoadGame_Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_Button> DeleteSave_Button;

private:
	UPROPERTY()
	TObjectPtr<class USlotEntryMetadata> SelectedSlotData = nullptr;
	
	UFUNCTION()
	virtual void Handle_DeleteSave();

	UFUNCTION()
	virtual void Handle_LoadGame();

	UFUNCTION()
	void Handle_SelectionChanged(UObject* SelectedItem);
};

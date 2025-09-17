#pragma once

#include "CoreMinimal.h"
#include "G_SaveLoad_Panel.h"
#include "SaveGameMenu.generated.h"

UCLASS(Abstract)
class RTS_API USaveGameMenu : public UG_SaveLoad_Panel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> SlotName_Input;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_Button> SaveGame_Button;

private:
	UFUNCTION()
	virtual void Handle_SaveButtonClick();
};

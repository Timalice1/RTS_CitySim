#pragma once

#include "CoreMinimal.h"
#include "UI/Generic/InteractionPanelWidget.h"
#include "Building_InteractionPanel.generated.h"

DECLARE_DELEGATE(FOnDestroyButtonClickedEvent);

UCLASS(Abstract)
class RTS_API UBuilding_InteractionPanel : public UInteractionPanelWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> DestroyButton = nullptr;
 
 
	FOnDestroyButtonClickedEvent OnDestroyButtonClicked;
	
protected:
	UFUNCTION()
	virtual void HandleClicked();
};

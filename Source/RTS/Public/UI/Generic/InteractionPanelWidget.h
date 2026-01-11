#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPanelWidget.generated.h"

UCLASS(Abstract)
class RTS_API UInteractionPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> Title = nullptr;

	// Options list, will vary between objects
};

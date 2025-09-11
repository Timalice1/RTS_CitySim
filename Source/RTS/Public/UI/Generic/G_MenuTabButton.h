#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_MenuTabButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabButtonClickedEvent, class UG_MenuTabButton*, Tab);

UCLASS(Abstract)
class RTS_API UG_MenuTabButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void HandleClicked();

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnTabButtonClickedEvent OnTabButtonClicked;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TabButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> TabIcon = nullptr;

	UPROPERTY(EditInstanceOnly, Category = TabButton)
	TSoftObjectPtr<class UTexture2D> IconTexture;
};

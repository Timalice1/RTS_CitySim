#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_Button.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedDelegate);

UCLASS(Abstract)
class RTS_API UG_Button : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnButtonClickedDelegate OnClicked;

private:
	UFUNCTION()
	virtual void Handle_Clicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> ButtonLabel;

	UPROPERTY(EditInstanceOnly, Category = G_Button)
	FText ButtonLabelText = FText::FromString(TEXT("button"));
};

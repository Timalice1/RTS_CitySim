#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_TextBlock.generated.h"

UCLASS(Abstract)
class RTS_API UG_TextBlock : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable, Category = G_TextBlock)
	virtual void SetText(const FText &InText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock = nullptr;

	UPROPERTY(EditInstanceOnly, Category = G_TextBlock)
	FText Text = FText::FromString(TEXT("placeholder"));
	UPROPERTY(EditInstanceOnly, Category = G_TextBlock)
	FName FontType = FName(TEXT("None"));
	UPROPERTY(EditInstanceOnly, Category = G_TextBlock)
	float FontSize = 25.f;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

UCLASS(Abstract)
class RTS_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void InitNotification(const FText& InText, const FName& Verbosity);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> WidgetBorder = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> NotificationHighlighter = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UG_TextBlock> NotificationText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> NotificationAnimation;

	UPROPERTY(EditDefaultsOnly, Category = NotificationConfig)
	TMap<FName, FLinearColor> VerbosityMapping;

	UFUNCTION(BlueprintCallable, Category = Notification)
	virtual void CloseNotification();
};

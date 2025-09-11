#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_Modals.generated.h"

enum ENotificationVerbosity : uint8;

/**
 * Modals layer widget
 * Uses for displaying popups, notifications or other in-game or system messages
 */
UCLASS()
class RTS_API UL_Modals : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UScrollBox> NotificationsContainer = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	TSoftClassPtr<class UNotificationWidget> NotificationWidget_Class;

public:
	UFUNCTION(BlueprintCallable, Category = Modals)
	virtual void PushNotification(const FString &InMsgText, const FName &Verbosity);
};

#include "UI/Layers/L_Modals.h"

#include "Components/ScrollBox.h"
#include "UI/Modals/NotificationWidget.h"

void UL_Modals::PushNotification(const FString& InMsgText, const FName &Verbosity)
{
	if (TSubclassOf<UNotificationWidget> notificationWidgetClass = NotificationWidget_Class.LoadSynchronous())
	{
		if (UNotificationWidget* notification = CreateWidget<UNotificationWidget>(GetOwningPlayer(), notificationWidgetClass))
		{
			notification->InitNotification(FText::FromString(InMsgText), Verbosity);
			NotificationsContainer->InsertChildAt(0, notification);
			NotificationsContainer->ScrollToEnd();
		}
	}
}

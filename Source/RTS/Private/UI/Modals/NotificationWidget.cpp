#include "UI/Modals/NotificationWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Generic/G_TextBlock.h"

void UNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseNotification);

	PlayAnimation(NotificationAnimation);
}

void UNotificationWidget::InitNotification(const FText& InText, const FName &Verbosity)
{
	NotificationText->SetText(InText);
	if (VerbosityMapping.Contains(Verbosity))
	{
		WidgetBorder->SetBrushColor(*VerbosityMapping.Find(Verbosity));
		NotificationHighlighter->SetColorAndOpacity(*VerbosityMapping.Find(Verbosity));
	}
}

void UNotificationWidget::CloseNotification()
{
	RemoveFromParent();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

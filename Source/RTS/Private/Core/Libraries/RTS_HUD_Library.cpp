#include "Core/Libraries/RTS_HUD_Library.h"
#include "UI/RTS_HUD.h"
#include "UI/Layers/L_Modals.h"

void URTS_HUD_Library::PushNotification(APlayerController* player, const FString& InMessage, const FName &Verbosity)
{
	checkf(player, TEXT("Player ref invalid"));
	if (ARTS_HUD* hud = Cast<ARTS_HUD>(player->GetHUD()))
	{
		if (hud->ModalsLayer())
			hud->ModalsLayer()->PushNotification(InMessage, Verbosity);
	}
}

void URTS_HUD_Library::TogglePauseMenu(APlayerController* player)
{
	checkf(player, TEXT("Player ref invalid"));
	if (ARTS_HUD* hud = Cast<ARTS_HUD>(player->GetHUD()))
		hud->TogglePauseMenu();
}

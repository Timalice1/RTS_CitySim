#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RTS_HUD_Library.generated.h"

UCLASS(meta = (BlueprintThreadSafe))
class RTS_API URTS_HUD_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Push notification message into Modals layer
	 * @param player Owning player controller
	 * @param InMessage Notification message text
	 * @param Verbosity Notification verbosity level (System, Warning, Success)
	 */
	UFUNCTION(BlueprintCallable, Category = RTS_HUD_FunctionLibrary)
	static void PushNotification(APlayerController* player, const FString& InMessage,
	                             const FName& Verbosity = FName("System"));

	UFUNCTION(BlueprintCallable, Category = RTS_HUD_FunctionLibrary)
	static void TogglePauseMenu(APlayerController* player);
};

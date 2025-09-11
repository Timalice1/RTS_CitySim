#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_Menus.generated.h"

/**
 * Menus layer widget
 * Used for pause menu, settings panel, save/load panel, exit game etc.
 */
UCLASS(Abstract)
class RTS_API UL_Menus : public UUserWidget
{
	GENERATED_BODY()
};

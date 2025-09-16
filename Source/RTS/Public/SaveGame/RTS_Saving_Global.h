#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RTS_Saving_Global.generated.h"

UCLASS()
class RTS_API URTS_Saving_Global : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> savingsSlots;
};
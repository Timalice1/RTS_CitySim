#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_GameState.generated.h"

/**
 * Game state class, for storing and managing world specific data
 */
UCLASS()
class RTS_API ARTS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};

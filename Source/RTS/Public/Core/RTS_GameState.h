#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SaveGame/RTS_SaveGame.h"
#include "RTS_GameState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRTSGameState, Log, All);

/**
 * Game state class, for storing and managing world specific data
 */
UCLASS()
class RTS_API ARTS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UWorld* WorldContext = nullptr;
	
	UFUNCTION()
	void OnSaveGameRequested(class URTS_SaveGame* SaveGameObject);

	UFUNCTION()
	void OnSaveGameLoaded(class URTS_SaveGame* SaveGameObject);
};

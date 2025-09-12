#include "Core/RTS_GameState.h"
#include "Core/RTS_GameInstance.h"
#include "SaveGame/RTS_SaveGame.h"

DEFINE_LOG_CATEGORY(LogRTSGameState);

void ARTS_GameState::BeginPlay()
{
	Super::BeginPlay();

	verify((WorldContext = GetWorld()) != nullptr);

	if (URTS_GameInstance* gi = Cast<URTS_GameInstance>(GetGameInstance()))
	{
		
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%s - game instance casted"), *GetName()));
#endif
		
		gi->OnSaveGameRequested.AddDynamic(this, &ThisClass::OnSaveGameRequested);
		gi->OnSaveLoaded.AddDynamic(this, &ThisClass::OnSaveGameLoaded);
	}
}

void ARTS_GameState::OnSaveGameRequested(URTS_SaveGame* SaveGameObject)
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%s - SaveGameRequested"), *GetName()));
#endif
}

void ARTS_GameState::OnSaveGameLoaded(URTS_SaveGame* SaveGameObject)
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%s - Save game loaded"), *GetName()));
#endif
}

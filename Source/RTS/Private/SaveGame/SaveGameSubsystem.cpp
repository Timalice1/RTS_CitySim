#include "SaveGame/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RTS_SaveGame.h"

static int32 INDEX_UserDefault = 0;

void USaveGameSubsystem::Request_SaveGame(const FString& toSlot)
{
	if (!DefaultSaveGameObject)
		return;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("GameInstance -> SaveGameRequested")));
#endif

	OnSaveGameRequested.Broadcast(DefaultSaveGameObject);

	FAsyncSaveGameToSlotDelegate save_Delegate;
	save_Delegate.BindUObject(this, &ThisClass::HandleGameSaved);
	UGameplayStatics::AsyncSaveGameToSlot(DefaultSaveGameObject, toSlot, INDEX_UserDefault, save_Delegate);
}

void USaveGameSubsystem::Request_LoadGame(const FString& fromSlot)
{
	if (!UGameplayStatics::DoesSaveGameExist(fromSlot, INDEX_UserDefault))
	{
		DefaultSaveGameObject = Cast<URTS_SaveGame>(UGameplayStatics::CreateSaveGameObject(URTS_SaveGame::StaticClass()));
		return;
	}

	FAsyncLoadGameFromSlotDelegate load_delegate;
	load_delegate.BindUObject(this, &ThisClass::HandleGameLoaded);
	UGameplayStatics::AsyncLoadGameFromSlot(fromSlot, INDEX_UserDefault, load_delegate);
}

void USaveGameSubsystem::HandleGameLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveObject)
{
	if (SaveObject)
	{
		verify((DefaultSaveGameObject = Cast<URTS_SaveGame>(SaveObject)) != nullptr);
		OnSaveLoaded.Broadcast(DefaultSaveGameObject);
	}
}

void USaveGameSubsystem::HandleGameSaved(const FString& SlotName, int UserIndex, bool bSuccess)
{
	if (bSuccess)
		OnGameSaved.Broadcast();
}
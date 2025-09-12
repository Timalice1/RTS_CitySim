#include "Core/RTS_GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/RTS_SaveGame.h"

static FString SLOT_Default = TEXT("slot_1");
static int32 INDEX_UserDefault = 0;

void URTS_GameInstance::Init()
{
	Super::Init();
	Request_LoadGame(SLOT_Default);
}

void URTS_GameInstance::Request_SaveGame(const FString& toSlot)
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

void URTS_GameInstance::Request_LoadGame(const FString& fromSlot)
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

void URTS_GameInstance::HandleGameLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveObject)
{
	if (SaveObject)
	{
		verify((DefaultSaveGameObject = Cast<URTS_SaveGame>(SaveObject)) != nullptr);
		OnSaveLoaded.Broadcast(DefaultSaveGameObject);
	}
}

void URTS_GameInstance::HandleGameSaved(const FString& SlotName, int UserIndex, bool bSuccess)
{
	if (bSuccess)
		OnGameSaved.Broadcast();
}

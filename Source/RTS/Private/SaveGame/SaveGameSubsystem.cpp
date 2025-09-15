#include "SaveGame/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveableInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static int32 INDEX_UserDefault = 0;

void USaveGameSubsystem::Request_SaveGame(const FString& toSlot)
{
	if (!DefaultSaveGameObject)
		DefaultSaveGameObject = Cast<URTS_SaveGame>(UGameplayStatics::CreateSaveGameObject(URTS_SaveGame::StaticClass()));
	OnSaveGameRequested.Broadcast(DefaultSaveGameObject);

	FAsyncSaveGameToSlotDelegate save_Delegate;
	save_Delegate.BindUObject(this, &ThisClass::HandleGameSaved);
	UGameplayStatics::AsyncSaveGameToSlot(DefaultSaveGameObject, toSlot, INDEX_UserDefault, save_Delegate);
}

void USaveGameSubsystem::Request_LoadGame(const FString& fromSlot)
{
	if (!UGameplayStatics::DoesSaveGameExist(fromSlot, INDEX_UserDefault))
		return;

	FAsyncLoadGameFromSlotDelegate load_delegate;
	load_delegate.BindUObject(this, &ThisClass::HandleGameLoaded);
	UGameplayStatics::AsyncLoadGameFromSlot(fromSlot, INDEX_UserDefault, load_delegate);
}

TArray<uint8> USaveGameSubsystem::SerializeObject(UObject* Target)
{
	TArray<uint8> bytes;
	FMemoryWriter memWriter(bytes, true);
	FObjectAndNameAsStringProxyArchive Ar(memWriter, false);
	Ar.ArIsSaveGame = true;
	Target->Serialize(Ar);

	if (ISaveableInterface* saveable = Cast<ISaveableInterface>(Target))
		saveable->SaveObjectData(Ar);
	return bytes;
}

void USaveGameSubsystem::DeserializeObject(UObject* Target, TArray<uint8> bytes)
{
	FMemoryReader memReader(bytes, true);
	FObjectAndNameAsStringProxyArchive Ar(memReader, false);
	Ar.ArIsSaveGame = true;
	Target->Serialize(Ar);

	if (ISaveableInterface* saveable = Cast<ISaveableInterface>(Target))
		saveable->LoadObjectData(Ar);
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
	{
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("GAME SAVED!")));
#endif
		OnGameSaved.Broadcast();
	}
}

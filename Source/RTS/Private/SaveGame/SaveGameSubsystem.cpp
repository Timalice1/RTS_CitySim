#include "SaveGame/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/RTS_Saving_Global.h"
#include "SaveGame/SaveableInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static int32 INDEX_UserDefault = 0;
static FString SLOT_Global = TEXT("save_global");

void USaveGameSubsystem::Request_SaveGame(const FString& toSlot)
{
	if (!SaveGame_Default)
		SaveGame_Default = Cast<URTS_SaveGame>(UGameplayStatics::CreateSaveGameObject(URTS_SaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(toSlot, INDEX_UserDefault))
	{
		// TODO: Push confirmation about save slot rewriting
	}

	OnSaveGameRequested.Broadcast(SaveGame_Default);

	FAsyncSaveGameToSlotDelegate save_Delegate;
	save_Delegate.BindUObject(this, &ThisClass::HandleGameSaved);
	UGameplayStatics::AsyncSaveGameToSlot(SaveGame_Default, toSlot, INDEX_UserDefault, save_Delegate);
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

void USaveGameSubsystem::AddSavingSlot(const FString& InSlotName)
{
	if (SaveGame_Globals && !SaveGame_Globals->savingsSlots.Contains(InSlotName))
	{
		SaveGame_Globals->savingsSlots.Add(InSlotName);
		UGameplayStatics::SaveGameToSlot(SaveGame_Globals, SLOT_Global, INDEX_UserDefault);
	}
}

TArray<FString> USaveGameSubsystem::LoadSavingsSlots() const
{
	if (UGameplayStatics::DoesSaveGameExist(SLOT_Global, INDEX_UserDefault))
		return Cast<URTS_Saving_Global>(UGameplayStatics::LoadGameFromSlot(SLOT_Global, INDEX_UserDefault))->savingsSlots;
	return TArray<FString>();
}

void USaveGameSubsystem::DeleteSaveSlot(const FString& InSlotName) const
{
	UGameplayStatics::DeleteGameInSlot(InSlotName, INDEX_UserDefault);
	SaveGame_Globals->savingsSlots.Remove(InSlotName);
	UGameplayStatics::SaveGameToSlot(SaveGame_Globals, SLOT_Global, INDEX_UserDefault);
}

void USaveGameSubsystem::LoadGlobalSave()
{
	if (!UGameplayStatics::DoesSaveGameExist(SLOT_Global, INDEX_UserDefault))
	{
		SaveGame_Globals = Cast<URTS_Saving_Global>(UGameplayStatics::CreateSaveGameObject(URTS_Saving_Global::StaticClass()));
		return;
	}
	SaveGame_Globals = Cast<URTS_Saving_Global>(UGameplayStatics::LoadGameFromSlot(SLOT_Global, INDEX_UserDefault));
}

void USaveGameSubsystem::HandleGameLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveObject)
{
	if (SaveObject)
	{
		verify((SaveGame_Default = Cast<URTS_SaveGame>(SaveObject)) != nullptr);
		OnSaveLoaded.Broadcast(SaveGame_Default);
	}
}

void USaveGameSubsystem::HandleGameSaved(const FString& SlotName, int UserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		// save slot name to global scope
		AddSavingSlot(SlotName);
		OnGameSaved.Broadcast();
	}
}

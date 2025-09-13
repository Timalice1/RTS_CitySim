#include "Core/RTS_GameState.h"

#include "EngineUtils.h"
#include "Core/RTS_GameInstance.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveableInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

DEFINE_LOG_CATEGORY(LogRTSGameState);

void ARTS_GameState::BeginPlay()
{
	Super::BeginPlay();

	verify((WorldContext = GetWorld()) != nullptr);

	if (URTS_GameInstance* gi = Cast<URTS_GameInstance>(GetGameInstance()))
	{
		gi->OnSaveGameRequested.AddDynamic(this, &ThisClass::OnSaveGameRequested);
		gi->OnSaveLoaded.AddDynamic(this, &ThisClass::OnSaveGameLoaded);
	}
}

void ARTS_GameState::OnSaveGameRequested(URTS_SaveGame* SaveGameObject)
{
	if (!SaveGameObject)
		return;

	SaveGameObject->ActorSaveRecords.Empty();
	for (FActorIterator It(WorldContext); It; ++It)
	{
		AActor* actor = *It;
		if (ISaveableInterface* saveable = Cast<ISaveableInterface>(actor))
		{
			FActorSaveDataRecord saveRecord;
			saveRecord.ActorClass = actor->GetClass();
			saveRecord.ActorName = actor->GetName();
			saveRecord.ActorTransform = actor->GetActorTransform();

			FMemoryWriter MemWriter(saveRecord.ByteData);
			FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
			Ar.ArIsSaveGame = true;

			saveable->SaveObjectData(Ar);
			actor->Serialize(Ar);
			SaveGameObject->ActorSaveRecords.Add(saveRecord);

			UE_LOG(LogRTSGameState, Warning, TEXT("SAVED: %s"), *actor->GetName())
		}
	}
}

void ARTS_GameState::OnSaveGameLoaded(URTS_SaveGame* SaveGameObject)
{
	if (!SaveGameObject)
		return;

	// Remove all saveable actors that already on the scene
	for (FActorIterator It(WorldContext); It; ++It)
	{
		if (AActor* actor = *It)
		{
			if (!actor->Implements<USaveableInterface>())
				continue;
			actor->Destroy();
		}
	}

	// Respawn saveable actors from game save
	for (FActorSaveDataRecord& saveRecord : SaveGameObject->ActorSaveRecords)
	{
		if (AActor* actor = WorldContext->SpawnActor<AActor>(saveRecord.ActorClass, saveRecord.ActorTransform))
		{
			if (ISaveableInterface* saveable = Cast<ISaveableInterface>(actor))
			{
				FMemoryReader MemReader(saveRecord.ByteData);
				FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
				Ar.ArIsSaveGame = true;
				actor->Serialize(Ar);

				saveable->LoadObjectData(Ar);

				UE_LOG(LogGameState, Warning, TEXT("LOADED: %s"), *actor->GetName());
			}
		}
	}
}

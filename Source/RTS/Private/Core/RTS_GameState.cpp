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
	{
		UE_LOG(LogRTSGameState, Error, TEXT("OnSaveGameRequested: Game save object is NULL"))
		return;
	}

	SaveGameObject->ActorSaveRecords.Empty();

	for (FActorIterator It(WorldContext); It; ++It)
	{
		AActor* actor = *It;
		// Skip actors that are not saveable
		if (!actor->Implements<USaveableInterface>())
			continue;

		FActorSaveDataRecord saveRecord;
		saveRecord.ActorName = actor->GetName();
		saveRecord.ActorClass = actor->GetClass();
		saveRecord.ActorTransform = actor->GetActorTransform();

		FMemoryWriter MemWriter(saveRecord.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		Ar.ArIsSaveGame = true; // Serialize only properties that are marked as SaveGame

		// Save actor-specific data
		ISaveableInterface::Execute_SaveObjectData(actor);

		// Serialize actor
		actor->Serialize(Ar);

		// Add new save record to save game object
		SaveGameObject->ActorSaveRecords.Add(saveRecord);
	}
}

void ARTS_GameState::OnSaveGameLoaded(URTS_SaveGame* SaveGameObject)
{
	if (!SaveGameObject)
	{
		UE_LOG(LogRTSGameState, Error, TEXT("OnSaveGameLoaded: Game save object is NULL"))
		return;
	}

	for (FActorSaveDataRecord& actorSaveRecord : SaveGameObject->ActorSaveRecords)
	{
		if (AActor* actor = WorldContext->SpawnActor<AActor>(actorSaveRecord.ActorClass, actorSaveRecord.ActorTransform))
		{
			FMemoryReader MemReader(actorSaveRecord.ByteData);
			FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
			Ar.ArIsSaveGame = true;

			ISaveableInterface::Execute_LoadObjectData(actor);

			actor->Serialize(Ar);
		}
	}
}

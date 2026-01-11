#include "Core/RTS_GameState.h"

#include "EngineUtils.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveableInterface.h"
#include "SaveGame/SaveGameSubsystem.h"

DEFINE_LOG_CATEGORY(LogRTSGameState);

void ARTS_GameState::BeginPlay()
{
	Super::BeginPlay();

	verify((WorldContext = GetWorld()) != nullptr);

	if (USaveGameSubsystem* sg_subsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		sg_subsystem->OnSaveGameRequested.AddDynamic(this, &ThisClass::OnSaveGameRequested);
		sg_subsystem->OnSaveLoaded.AddDynamic(this, &ThisClass::OnSaveGameLoaded);
	}
}

void ARTS_GameState::OnSaveGameRequested(URTS_SaveGame* SaveGameObject)
{
	if (!SaveGameObject)
		return;
	
	SaveGameObject->WorldActorsRecords.Empty();
	for (FActorIterator It(WorldContext); It; ++It)
	{
		AActor* actor = *It;
		if (!actor || !actor->Implements<USaveableInterface>())
			continue;

		FActorSaveDataRecord saveRecord;
		saveRecord.ActorClass = actor->GetClass();
		saveRecord.ActorName = actor->GetName();
		saveRecord.ActorTransform = actor->GetActorTransform();
		saveRecord.ByteData = GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->SerializeObject(actor);

		SaveGameObject->WorldActorsRecords.Add(saveRecord);
		UE_LOG(LogRTSGameState, Warning, TEXT("SAVED: %s"), *actor->GetName())
	}
}

void ARTS_GameState::OnSaveGameLoaded(URTS_SaveGame* SaveGameObject)
{
	if (!SaveGameObject)
		return;

	// Remove all saveable actors that already on the scene
	for (FActorIterator It(WorldContext); It; ++It)
	{
		AActor* actor = *It;
		if (actor && actor->Implements<USaveableInterface>())
			actor->Destroy();
	}

	// Respawn saveable actors from game save
	for (FActorSaveDataRecord& saveRecord : SaveGameObject->WorldActorsRecords)
	{
		if (AActor* actor = WorldContext->SpawnActor<AActor>(saveRecord.ActorClass, saveRecord.ActorTransform))
		{
			GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->DeserializeObject(actor, saveRecord.ByteData);
			UE_LOG(LogGameState, Warning, TEXT("LOADED: %s"), *actor->GetName());
		}
	}
}

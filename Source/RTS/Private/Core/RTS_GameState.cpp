#include "Core/RTS_GameState.h"

#include "EngineUtils.h"
#include "SaveGame/RTS_SaveGame.h"
#include "SaveGame/SaveableInterface.h"
#include "SaveGame/SaveGameSubsystem.h"

DEFINE_LOG_CATEGORY(LogRTSGameState);

ARTS_GameState::ARTS_GameState()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

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
	
	// Clean up currently saved actors
	// TODO: Remove only dirty actors instead
	SaveGameObject->WorldActorsRecords.Empty();

	// Iterate through each actor on the scene 
	for (FActorIterator It(WorldContext); It; ++It)
	{
		// Filter actors by saveable interface
		AActor* actor = *It;
		if (!actor || !actor->Implements<USaveableInterface>())
			continue;

		// For each saveable actor, create a save record to serialize to binary
		FActorSaveDataRecord saveRecord;
		saveRecord.ActorClass = actor->GetClass();
		saveRecord.ActorName = actor->GetName();
		saveRecord.ActorTransform = actor->GetActorTransform();
		
		// Run internal actor serialization logic
		// Serialize fields, marked as UPROPERTY(SaveGame)
		saveRecord.ByteData = GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->SerializeObject(actor);

		// Write this record to the file for serialization
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

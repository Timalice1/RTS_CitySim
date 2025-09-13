#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RTS_SaveGame.generated.h"

/// Generic struct that represents actor saved data record
USTRUCT()
struct FActorSaveDataRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString ActorName;

	UPROPERTY(SaveGame)
	UClass* ActorClass;

	UPROPERTY(SaveGame)
	FTransform ActorTransform;

	UPROPERTY(SaveGame)
	TArray<uint8> ByteData;
};

/// Default save game object, used for saving game-specific data
UCLASS(Abstract)
class RTS_API URTS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FActorSaveDataRecord> WorldActorsRecords;
};

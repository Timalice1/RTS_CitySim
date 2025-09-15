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

/// Actor component save data record
USTRUCT()
struct FComponentSaveDataRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* componentClass;

	UPROPERTY(SaveGame)
	TArray<uint8> bytes;
};

/// Default save game object, used for saving game-specific data
UCLASS(Abstract)
class RTS_API URTS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FActorSaveDataRecord> WorldActorsRecords;

	UPROPERTY()
	TArray<FComponentSaveDataRecord> componentsData;
};

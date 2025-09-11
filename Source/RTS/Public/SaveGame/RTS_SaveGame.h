#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RTS_SaveGame.generated.h"

/// Generic world actor data for saving
/// Used for save data of the actor, that are placed on the map during gameplay
/// And respawn this actor on game save is loaded
/// NOTE: are not store actor-specific data, only data that needs to respawn this actor
USTRUCT()
struct FWorldActor
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* ObjectClass;
	
	UPROPERTY()
	FTransform ObjectTransform;

	FWorldActor(UClass* InObjectClass, const FTransform& InObjectTransform = FTransform())
		: ObjectClass(InObjectClass), ObjectTransform(InObjectTransform)
	{
	}

	FWorldActor()
		: ObjectClass(nullptr), ObjectTransform(FTransform())
	{
	}
};

/// Default SaveGame object
UCLASS()
class RTS_API URTS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	URTS_SaveGame()
	{
	}

	UPROPERTY(VisibleAnywhere, Category = WorldData)
	TArray<FWorldActor> WorldActors;

	UPROPERTY(VisibleAnywhere, Category = RoadData)
	TArray<FTransform> InstancesTransforms;
};

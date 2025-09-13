#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSavedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveLoadedDelegate, class URTS_SaveGame*, SaveGameObject);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameRequestedDelegate, class URTS_SaveGame*, SaveGameObject);

UCLASS()
class RTS_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Request_SaveGame(const FString& toSlot);
	virtual void Request_LoadGame(const FString& fromSlot);

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnGameSavedDelegate OnGameSaved;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnSaveGameRequestedDelegate OnSaveGameRequested;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnSaveLoadedDelegate OnSaveLoaded;

private:
	UPROPERTY()
	class URTS_SaveGame* DefaultSaveGameObject = nullptr;
	
	UFUNCTION()
	void HandleGameLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveObject);

	UFUNCTION()
	void HandleGameSaved(const FString& SlotName, int UserIndex, bool bSuccess);
};

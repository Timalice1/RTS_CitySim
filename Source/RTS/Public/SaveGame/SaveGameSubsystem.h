#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSavedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveLoadedDelegate, class URTS_SaveGame*, SaveGameObject);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameRequestedDelegate, class URTS_SaveGame*, SaveGameObject);


UCLASS(BlueprintType, Blueprintable)
class RTS_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/// Forcing async save game request to specific slot
	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	virtual void Request_SaveGame(const FString& toSlot);

	/// Forcing async save loading from specific slot
	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	virtual void Request_LoadGame(const FString& fromSlot);


	/// Serializes target object into bytes array
	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	TArray<uint8> SerializeObject(UObject* Target);

	/// Deserialize target object from bytes array
	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	void DeserializeObject(UObject* Target, TArray<uint8> bytes);

	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	virtual TArray<FString> LoadSavingsSlots() const;

	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	virtual void DeleteSaveSlot(const FString& InSlotName) const;

	UFUNCTION(BlueprintCallable, Category = SaveGameSubsystem)
	virtual void LoadGlobalSave();

	/// Called after successful game saving
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnGameSavedDelegate OnGameSaved;

	/// Called on save game is requested
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnSaveGameRequestedDelegate OnSaveGameRequested;

	/// Called on game save is successfully loaded
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnSaveLoadedDelegate OnSaveLoaded;

protected:
	virtual void AddSavingSlot(const FString& InSlotName);

private:
	UPROPERTY()
	class URTS_SaveGame* SaveGame_Default = nullptr;

	UPROPERTY()
	class URTS_Saving_Global* SaveGame_Globals = nullptr;

	UFUNCTION()
	void HandleGameLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveObject);

	UFUNCTION()
	void HandleGameSaved(const FString& SlotName, int UserIndex, bool bSuccess);
};

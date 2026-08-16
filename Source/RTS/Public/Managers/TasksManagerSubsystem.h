#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TasksManagerSubsystem.generated.h"

class UTask;

DECLARE_LOG_CATEGORY_EXTERN(LogTasksManager, Log, All);

UCLASS()
class RTS_API UTasksManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/* Register newly created task in the manager */
	UFUNCTION(BlueprintCallable, Category = TasksManager)
	virtual void RegisterTask(UTask* InTask);

	/*  
	 * Return a task object reference by task ID
	 * Used for a save/load system
	 */
	virtual UTask* GetTaskByID(const FGuid& InGuid) const;

	/// Returns most scored task for provided unit
	/// 
	/// Evaluates tasks score from a backlog, based on Task priority, 
	/// unit priorities, preferences and skills
	virtual UTask* RequestTaskForUnit(const class ARTS_BaseUnit* Unit);

private:
	UPROPERTY()
	TArray<UTask*> Tasks;

	UPROPERTY()
	class USaveGameSubsystem* _saveGameManager = nullptr;

	UFUNCTION()
	virtual void Handle_SaveRequested(class URTS_SaveGame* SaveGameObject);

	UFUNCTION()
	virtual void Handle_SaveLoaded(class URTS_SaveGame* SaveGameObject);

	UFUNCTION()
	virtual void Handle_TaskFinished(class UTask* Task, bool bSuccess, const FString& message);

	UFUNCTION()
	virtual void OnUnitAvailable(class ARTS_BaseUnit* InUnit);
};

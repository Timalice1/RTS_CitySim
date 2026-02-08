#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TasksManagerSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTaskCreatedEvent, const class UTask*, Task);

class UTask;

UCLASS()
class RTS_API UTasksManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual FGuid RegisterTask(UTask* InTask);

	UTask* GetTaskByID(const FGuid& InGuid) const;

	FOnTaskCreatedEvent OnTaskCreated;

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
	virtual void Handle_TaskCompleted(class UTask* Task);
};

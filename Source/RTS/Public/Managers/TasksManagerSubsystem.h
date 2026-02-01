#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TasksManagerSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTaskCreatedEvent, const UTask*, Task);

class UTask;

UCLASS()
class RTS_API UTasksManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void AddTask(const UTask* InTask);

	FOnTaskCreatedEvent OnTaskCreated;
};

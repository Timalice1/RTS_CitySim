#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Task.h"
#include "Navigation/PathFollowingComponent.h"
#include "Task_Collect.generated.h"

UCLASS()
class RTS_API UTask_Collect : public UTask
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<class AItemActor> TargetItemType;

	UPROPERTY()
	int TargetAmount;

private:
	UFUNCTION()
	virtual void Execute_Implementation(class AAIController* Executor) override;

	virtual bool IsAvailableFor(const class ARTS_BaseUnit* InUnit) override;

	virtual void MoveToSource();

	UFUNCTION()
	void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	virtual void FinishTask(const bool Success, const FString& message);

	UPROPERTY()
	TArray<struct FItemSearchResult> _targetSources;
	int _currentSource = 0;
};

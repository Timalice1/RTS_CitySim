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
	TSubclassOf<class AActor> TargetItemType;

	UPROPERTY()
	uint8 TargetAmount;

private:
	UFUNCTION()
	virtual void Execute_Implementation(class AAIController* Executor) override;

	virtual bool IsAvailableFor(const class ARTS_BaseUnit* InUnit) override;

	UFUNCTION()
	virtual void MoveToItem();

	UFUNCTION()
	void OnItemReached(AActor* OverlappedActor, AActor* OtherActor);
	void OnTargetReached(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult);

	UPROPERTY()
	TArray<class AActor*> _targetItems;
};

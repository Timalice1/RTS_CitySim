#pragma once

#include "CoreMinimal.h"
#include "Task.h"
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
	
	UFUNCTION()
	virtual void MoveToItem();
	
	UPROPERTY()
	TArray<class AActor*> _targetItems;
	
	UPROPERTY()
	TObjectPtr<class AAIController> _executor = nullptr;
};

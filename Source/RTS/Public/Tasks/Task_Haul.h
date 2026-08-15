#pragma once

#include "CoreMinimal.h"
#include "Task.h"
#include "Task_Haul.generated.h"

UCLASS()
class RTS_API UTask_Haul : public UTask
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), Category = Task_Haul)
	uint8 MaxStackSize = 5;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), Category = Task_Haul)
	FLinearColor DebugColor = FLinearColor::White;

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), Category = Task_Haul)
	TSubclassOf<class AActor> TargetItemType;
	
	// TODO: Change generic AActor ref to Item class
	UFUNCTION(BlueprintCallable, Category = Task_Haul)
	virtual bool TryRegisterItem(class AActor* InItem);

private:
	UFUNCTION()
	virtual void Execute_Implementation(class AAIController* Executor) override;

	virtual void MoveToItem();
	UFUNCTION()
	virtual void OnTargetReached(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY()
	TArray<class AActor*> _registeredItems;
	bool _bClosed = false;
	uint8 _targetIndex = 0;

};

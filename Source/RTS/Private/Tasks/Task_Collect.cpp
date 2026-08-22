#include "Tasks/Task_Collect.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/RTS_BaseUnit.h"
#include "Items/ItemActor.h"
#include "Components/InventoryComponent.h"


bool UTask_Collect::IsAvailableFor(const ARTS_BaseUnit* InUnit)
{
	if (!Super::IsAvailableFor(InUnit) || !TargetItemType || TargetAmount == 0)
		return false;

	uint8 trim = TargetAmount;
	if (const auto& inventory = InUnit->GetComponentByClass<UInventoryComponent>())
		trim = FMath::Min(inventory->GetAvailableCapacity(), TargetAmount);
	if (trim == 0) return false;

	// TODO: Add ResourcesManager world subsystem, instead of iterating through all actors of type each time
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetItemType, outActors);
	if (outActors.Num() < TargetAmount)
		return false;

	for (AActor* actor : outActors) {
		AItemActor* item = Cast<AItemActor>(actor);
		if (IsValid(item) && !item->IsReserved() && InUnit->IsPointReachable(item->GetActorLocation()))
			_targetItems.Add(item);
	}

	const FVector unitLocation = InUnit->GetActorLocation();
	_targetItems.Sort([&unitLocation](const AActor& A, const AActor& B)
	{
		const float targetA = FVector::DistSquared(A.GetActorLocation(), unitLocation);
		const float targetB = FVector::DistSquared(B.GetActorLocation(), unitLocation);
		return targetA < targetB;
	});

	if (_targetItems.Num() > trim)
		_targetItems.SetNum(trim);

	if (_targetItems.IsEmpty()) return false;

	return true;
}

void UTask_Collect::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);

	for (AItemActor* target : _targetItems)
		target->TryReserve();
	MoveToItem();
}

void UTask_Collect::MoveToItem()
{
	if (AActor* target = _targetItems[0]) {
		GetExecutor()->MoveToActor(target);
		target->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnItemReached);
	}
}

void UTask_Collect::OnItemReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!GetExecutor() || OtherActor != GetExecutor()->GetPawn())
		return;
	AItemActor* target = Cast<AItemActor>(OverlappedActor);
	if (!target)
		return;

	target->OnActorBeginOverlap.RemoveAll(this);

	if (const auto& inventory = GetExecutor()->GetPawn()->GetComponentByClass<UInventoryComponent>()) {
		if (!inventory->TryAddItem(target->GetClass())) {
			OnTaskFinished.Broadcast(this, false, FString("Failed to add item to inventory"));
			return;
		}

		_targetItems.Remove(target);
		target->PickUp();

		if (!_targetItems.IsEmpty()) {
			MoveToItem();
			return;
		}

		// Carry collected items to the target location
		GetExecutor()->MoveToLocation(TaskLocation);
		GetExecutor()->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ThisClass::OnTargetReached);
	}
}

void UTask_Collect::OnTargetReached(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult)
{
	GetExecutor()->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
	if (PathFollowingResult.IsSuccess())
		OnTaskFinished.Broadcast(this, true, FString("Task finished"));
}

#include "Tasks/Task_Collect.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/RTS_BaseUnit.h"
#include "Items/ItemActor.h"
#include "Components/InventoryComponent.h"
#include "Managers/RecoursesManagerSubsystem.h"


bool UTask_Collect::IsAvailableFor(const ARTS_BaseUnit* InUnit)
{
	if (!Super::IsAvailableFor(InUnit) || !TargetItemType || TargetAmount == 0)
		return false;

	int trim = TargetAmount;
	if (const auto& inventory = InUnit->GetComponentByClass<UInventoryComponent>())
		trim = FMath::Min(inventory->GetAvailableCapacity(), TargetAmount);
	if (trim == 0) return false;

	URecoursesManagerSubsystem* recoursesManager = GetWorld()->GetSubsystem<URecoursesManagerSubsystem>();
	if (!recoursesManager) return false;

	TArray<FItemSearchResult> result = recoursesManager->FindItemsByType(TargetItemType);
	if (result.IsEmpty()) return false;

	// Filter out unreachable sources
	result.RemoveAll([&InUnit](const FItemSearchResult& Source)
	{
		return !InUnit->IsPointReachable(Source.GetLocation());
	});

	// Sort by source priority and distance. Loose items first, storage as a fallback.
	const FVector unitLocation = InUnit->GetActorLocation();
	result.Sort([&unitLocation](const FItemSearchResult& A, const FItemSearchResult& B)
	{
		const int priorityA = A.ItemActor ? 0 : 1; // 0 for placed item, 1 for storage
		const int priorityB = B.ItemActor ? 0 : 1;

		if (priorityA != priorityB)
			return priorityA < priorityB;
		return FVector::DistSquared(A.GetLocation(), unitLocation) <
			FVector::DistSquared(B.GetLocation(), unitLocation);
	});

	int accumulated = 0;
	for (FItemSearchResult source : result) {
		if (accumulated >= trim) break;

		const int stillNeed = trim - accumulated;
		source.Requested = FMath::Min(stillNeed, source.Quantity);
		accumulated += source.Requested;

		_targetSources.Add(source);
	}

	return !_targetSources.IsEmpty();
}

void UTask_Collect::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);

	for (FItemSearchResult source : _targetSources) {
		if (source.ItemActor)
			source.ItemActor->TryReserve();
		if (source.Storage) {
			// TODO: Reserve a required amount of items in storage. See a InventoryComponent
			// NOTE: Waits for a storage implementation
		}
	}
	_currentSource = 0;
	GetExecutor()->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveComplete);
	MoveToSource();
}

void UTask_Collect::MoveToSource()
{
	GetExecutor()->MoveToLocation(_targetSources[_currentSource].GetLocation());
}

void UTask_Collect::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (_targetSources.IsValidIndex(_currentSource)) {
		const FItemSearchResult source = _targetSources[_currentSource];
		UInventoryComponent* unitInventory = GetExecutor()->GetPawn()->GetComponentByClass<UInventoryComponent>();

		if (source.ItemActor) {
			AItemActor* target = source.ItemActor;
			if (unitInventory->TryAddItem(TargetItemType))
				target->PickUp();
		}

		if (source.Storage) {
			const int removed = source.Storage->RemoveItem(TargetItemType, source.Requested);
			for (int i = 0; i < removed; i++)
				unitInventory->TryAddItem(TargetItemType);
		}

		_currentSource++;
		if (_targetSources.IsValidIndex(_currentSource)) {
			MoveToSource();
			return;
		}
		GetExecutor()->MoveToLocation(TaskLocation);
		return;
	}
	FinishTask(true, TEXT("Task finished"));
}

void UTask_Collect::FinishTask(const bool Success, const FString& message)
{
	GetExecutor()->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::OnMoveComplete);
	OnTaskFinished.Broadcast(this, Success, message);
}

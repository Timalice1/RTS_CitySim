#include "Tasks/Task_Collect.h"
#include "AIController.h"
#include "Core/Interfaces/InventoryInterface.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/RTS_BaseUnit.h"

void UTask_Collect::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);

	// Sort target items by distance to unit
	_targetItems.Sort([Executor](const AActor& A, const AActor& B)
	{
		const FVector unitLocation = Executor->GetPawn()->GetActorLocation();
		const float targetA = FVector::DistSquared(A.GetActorLocation(), unitLocation);
		const float targetB = FVector::DistSquared(B.GetActorLocation(), unitLocation);
		return targetA < targetB;
	});

	// Trim sorted array by target items amount
	uint8 trim = TargetAmount;
	if (const auto& inventory = Cast<IInventoryInterface>(Executor->GetPawn()))
		trim = FMath::Min(inventory->GetAvailableCapacity(), TargetAmount);
	if (_targetItems.Num() > trim)
		_targetItems.SetNum(trim);

	MoveToItem();
}

bool UTask_Collect::TryAssign(const ARTS_BaseUnit* InUnit)
{
	if (!Super::TryAssign(InUnit) || !TargetItemType || TargetAmount == 0)
		return false;

	// Check if the unit inventory isn't already full
	if (!InUnit->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass())
		|| Cast<IInventoryInterface>(InUnit)->GetAvailableCapacity() == 0)
		return false;

	// TODO: Add ResourcesManager world subsystem, instead of iterating through all actors of type each time
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetItemType, _targetItems);
	if (_targetItems.IsEmpty() || _targetItems.Num() < TargetAmount)
		return false;

	// Check items location accessibility
	for (AActor* item : _targetItems)
	{
		if (!InUnit->IsPointReachable(IsValid(item)? item->GetActorLocation() : FVector()))
			_targetItems.RemoveSwap(item);
	}
	if (_targetItems.IsEmpty() || _targetItems.Num() < TargetAmount)
		return false;

	return true;
}

void UTask_Collect::MoveToItem()
{
	if (AActor* target = _targetItems[0])
	{
		GetExecutor()->MoveToActor(target);
		target->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnItemReached);
	}
}

void UTask_Collect::OnItemReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!GetExecutor() || OtherActor != GetExecutor()->GetPawn())
		return;
	OverlappedActor->OnActorBeginOverlap.RemoveAll(this);

	if (IInventoryInterface* unitInventory = Cast<IInventoryInterface>(GetExecutor()->GetPawn()))
	{
		if (!unitInventory->TryAddItem(OverlappedActor->GetClass()))
		{
			OnTaskFinished.Broadcast(this, false, FString("Failed to add item to inventory"));
			return;
		}

		_targetItems.Remove(OverlappedActor);
		OverlappedActor->Destroy(); // TODO: replace with a PickUp() method on the item actor side

		if (!_targetItems.IsEmpty())
		{
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

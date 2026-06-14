#include "Tasks/Task_Haul.h"

#include "AIController.h"
#include "Core/Interfaces/InventoryInterface.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"

bool UTask_Haul::TryRegisterItem(AActor* InItem)
{
	if (_bClosed || _registeredItems.Num() >= MaxStackSize)
		return false;

	// Check out input item type
	if (!_registeredItems.IsEmpty())
	{
		if (_registeredItems[0]->GetClass() != InItem->GetClass())
			return false;
	}

	_registeredItems.Add(InItem);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), InItem->GetActorLocation(),
	                                    InItem->GetActorLocation() + FVector::UpVector * 1000.f, DebugColor, 50.f, 5.f);
	return true;
}

void UTask_Haul::Execute_Implementation(AAIController* Executor)
{
	Super::Execute_Implementation(Executor);
	_executor = Executor;
	_bClosed = true;
	MoveToItem();
}

void UTask_Haul::OnTargetReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IInventoryInterface* inventory = Cast<IInventoryInterface>(_executor->GetPawn()))
	{
		if (inventory->TryAddItem(OverlappedActor->GetClass()))
		{
			_registeredItems.Remove(OverlappedActor);
			OverlappedActor->Destroy();
			if (!_registeredItems.IsEmpty())
			{
				MoveToItem();
				return;
			}
			OnTaskFinished.Broadcast(this, true, FString("Task completed"));
			return;
		}
		OnTaskFinished.Broadcast(this, false, FString("Unable to add new item to inventory"));
	}
}

void UTask_Haul::MoveToItem()
{
	float bestDistance = FLT_MAX;
	AActor* target = nullptr;

	// Maybe split target items on a map by clusters and check the distance to the cluster center instead of the distance to pawn 
	for (AActor* current : _registeredItems)
	{
		const float currentDist = FVector::Dist(_executor->GetPawn()->GetActorLocation(), current->GetActorLocation());
		if (currentDist < bestDistance)
		{
			bestDistance = currentDist;
			target = current;
		}
	}

	if (target)
	{
		_executor->MoveToActor(target);
		target->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnTargetReached);
	}
}

#include "Tasks/Task_Collect.h"

#include "AIController.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

void UTask_Collect::Execute_Implementation(class AAIController* Executor)
{
	Super::Execute_Implementation(Executor);

	_executor = Executor;
	if (TargetItemType)
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetItemType, _targetItems);

	_targetItems.Sort([Executor](const AActor& A, const AActor& B)
	{
		const FVector unitLocation = Executor->GetPawn()->GetActorLocation();
		const float targetA = FVector::DistSquared(A.GetActorLocation(), unitLocation);
		const float targetB = FVector::DistSquared(B.GetActorLocation(), unitLocation);
		return targetA < targetB;
	});
	_targetItems.SetNum(TargetAmount);

	FLinearColor _dbg = FLinearColor::MakeRandomColor();
	for (const AActor* item : _targetItems)
	{
		if (GetWorld() && item)
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), item->GetActorLocation(), item->GetActorLocation() + FVector::UpVector * 1000, _dbg, 50.f, 5.f);
	}
}

void UTask_Collect::MoveToItem()
{
	// 	float bestDistance = FLT_MAX;
	// 	AActor* target = nullptr;
	//
	// 	// Maybe split target items on a map by clusters and check the distance to the cluster center instead of the distance to pawn 
	// 	for (AActor* current : _targetItems)
	// 	{
	// 		const float currentDist = FVector::Dist(_executor->GetPawn()->GetActorLocation(), current->GetActorLocation());
	// 		if (currentDist < bestDistance)
	// 		{
	// 			bestDistance = currentDist;
	// 			target = current;
	// 		}
	// 	}
	//
	// 	if (target)
	// 	{
	// 		_executor->MoveToActor(target);
	// 		target->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnTargetReached);
	// 	}
}

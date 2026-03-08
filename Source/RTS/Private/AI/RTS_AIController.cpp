#include "AI/RTS_AIController.h"

#include "AI/RTS_BaseUnit.h"
#include "BehaviorTree/BehaviorTree.h"

ARTS_AIController::ARTS_AIController()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ARTS_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARTS_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (const ARTS_BaseUnit* unit = Cast<ARTS_BaseUnit>(InPawn))
	{
		if (UBehaviorTree* btAsset = unit->GetBTAsset())
			RunBehaviorTree(btAsset);
	}
}

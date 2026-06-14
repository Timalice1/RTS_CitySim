#include "AI/RTS_AIController.h"
#include "AI/RTS_BaseUnit.h"

ARTS_AIController::ARTS_AIController()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ARTS_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	verify((_controlledUnit = Cast<ARTS_BaseUnit>(InPawn)));
	if (UBehaviorTree* defaultBT = _controlledUnit->GetDefaultBTAsset())
		RunBehaviorTree(defaultBT);
}

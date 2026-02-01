#include "AI/RTS_AIController.h"

ARTS_AIController::ARTS_AIController()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ARTS_AIController::BeginPlay()
{
	Super::BeginPlay();
}


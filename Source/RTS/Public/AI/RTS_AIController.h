#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTS_AIController.generated.h"

struct FTaskData;

UCLASS()
class RTS_API ARTS_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ARTS_AIController();
	virtual void BeginPlay() override;
};

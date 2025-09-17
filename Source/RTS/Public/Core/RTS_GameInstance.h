#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RTS_GameInstance.generated.h"

UCLASS()
class RTS_API URTS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};

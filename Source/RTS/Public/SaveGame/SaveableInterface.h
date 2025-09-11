#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveableInterface.generated.h"

UINTERFACE()
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

class URTS_SaveGame;

/**
 * Saveable interface, that should be implemented by all classes, which needs to be saved
 */
class RTS_API ISaveableInterface
{
	GENERATED_BODY()

public:
	virtual void SaveObjectData(URTS_SaveGame* SaveGameObject) = 0;
	virtual void LoadObjectData(URTS_SaveGame* SaveGameObject) = 0;
};

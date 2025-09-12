#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveableInterface.generated.h"

UINTERFACE()
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

/// Saveable interface, that all actors/components/object that can be saved should implement
class RTS_API ISaveableInterface
{
	GENERATED_BODY()

public:
	/// Serialize object-specific data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Saveable)
	void SaveObjectData();
	
	/// Load object-specific data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Saveable)
	void LoadObjectData();
};

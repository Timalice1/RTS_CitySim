#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectableInterface.generated.h"

UINTERFACE()
class USelectableInterface : public UInterface
{
	GENERATED_BODY()
};

class RTS_API ISelectableInterface
{
	GENERATED_BODY()
	
public:
	virtual void Select() = 0;
	virtual void Deselect() = 0;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UCLASS()
class RTS_API AItemActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class USphereComponent> ItemCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStaticMeshComponent> ItemMesh = nullptr;

public:
	AItemActor();

	UFUNCTION()
	virtual void PickUp();

	UFUNCTION()
	virtual bool TryReserve();

	UFUNCTION()
	virtual bool IsReserved() const { return bReserved; }

private:
	bool bReserved = false;
};

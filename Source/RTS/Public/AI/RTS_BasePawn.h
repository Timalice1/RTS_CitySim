#pragma once

#include "CoreMinimal.h"
#include "UnitsInterface.h"
#include "GameFramework/Pawn.h"
#include "SaveGame/SaveableInterface.h"
#include "RTS_BasePawn.generated.h"

UCLASS()
class RTS_API ARTS_BasePawn : public APawn,
                              public ISaveableInterface,
                              public IUnitsInterface
{
	GENERATED_BODY()

public:
	ARTS_BasePawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//~ Begin Saveable interface
	virtual void SaveObjectData(FArchive& Ar) override {};
	virtual void LoadObjectData(FArchive& Ar) override {}
	//~ End Saveable interface

	//~ Begin Units interface
	virtual TEnumAsByte<EUnitStatus> GetUnitStatus() const override { return _currentStatus; }
	virtual void SetUnitStatus(const TEnumAsByte<EUnitStatus> InNewStatus) override { _currentStatus = InNewStatus; }
	virtual void SetCurrentTask(class UTask* InTask) override { _currentTask = InTask; }
	//~ End units interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class USkeletalMeshComponent> BodyMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UFloatingPawnMovement> PawnMovement = nullptr;

	UPROPERTY()
	TEnumAsByte<EUnitStatus> _currentStatus = Idle;

	UPROPERTY()
	class UTask* _currentTask = nullptr;
};

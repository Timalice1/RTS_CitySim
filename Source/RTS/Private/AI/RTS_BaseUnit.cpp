#include "AI/RTS_BaseUnit.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/TasksManagerSubsystem.h"
#include "Managers/UnitsManagerSubsystem.h"
#include "Tasks/Task.h"

static FName NAME_CurrentTask = FName("CurrentTask");

ARTS_BaseUnit::ARTS_BaseUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseControllerRotationYaw = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //-->!! IMPORTANT FOR SAVE/LOAD !!<--

	// Setup mesh component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	BodyMesh->SetupAttachment(RootComponent);

	// Setup movement component
	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
}

void ARTS_BaseUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	verify((_controller = Cast<AAIController>(NewController)) != nullptr);

	// Register this unit in the Units manager
	GetWorld()->GetSubsystem<UUnitsManagerSubsystem>()->RegisterUnit(this);
}

void ARTS_BaseUnit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	int i = 10;
	for (const auto& item : _inventory)
	{
		UKismetSystemLibrary::DrawDebugString(
			GetWorld(),
			GetActorLocation() + FVector::UpVector * (300 + i),
			FString::Printf(TEXT("%s: %d"), *item.Key->GetName(), item.Value),
			0, FLinearColor::Blue, 0
		);
		i += 50;
	}
}

void ARTS_BaseUnit::AssignTask(UTask* InTask)
{
	if (!InTask)
		return;
	_currentTask = InTask;
	_currentTaskID = InTask->TaskID;
	InTask->OnTaskFinished.AddDynamic(this, &ThisClass::Handle_TaskFinished);

	UpdateBlackboardTask();
}

void ARTS_BaseUnit::Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message)
{
	if (bSuccess)
	{
		_currentTask = nullptr;
		_currentTaskID.Invalidate();
		UpdateBlackboardTask(); // invalidate blackboard value
	}
	OnUnitBecomeAvailable.Broadcast(this);
}

class UBehaviorTree* ARTS_BaseUnit::GetDefaultBTAsset() const
{
	if (BehaviorTreeAsset)
		return BehaviorTreeAsset.LoadSynchronous();
	return nullptr;
}

void ARTS_BaseUnit::UpdateBlackboardTask()
{
	if (UBlackboardComponent* blackboard = _controller->GetBlackboardComponent())
		blackboard->SetValueAsObject(NAME_CurrentTask, _currentTask);
}

bool ARTS_BaseUnit::TryAddItem(const TSubclassOf<class AActor>& InItem)
{
	if (_currentItemsCount < MaxInventoryCapacity)
	{
		_inventory.FindOrAdd(InItem)++;
		_currentItemsCount++;
		return true;
	}
	return false;
}

void ARTS_BaseUnit::LoadObjectData(FArchive& Ar)
{
	//Load and restore assigned task for that unit
	if (_currentTaskID.IsValid())
	{
		if (UTask* task = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->GetTaskByID(_currentTaskID))
			AssignTask(task);
	}
}

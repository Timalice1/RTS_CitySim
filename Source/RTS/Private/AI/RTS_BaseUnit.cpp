#include "AI/RTS_BaseUnit.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Managers/TasksManagerSubsystem.h"
#include "Managers/UnitsManagerSubsystem.h"
#include "Tasks/Task.h"

static FName NAME_CurrentTask = FName("CurrentTask");

ARTS_BaseUnit::ARTS_BaseUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bUseControllerRotationYaw = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Setup mesh component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	BodyMesh->SetupAttachment(RootComponent);

	// Setup movement component
	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
}

void ARTS_BaseUnit::BeginPlay()
{
	Super::BeginPlay();
	UE_VLOG(GetWorld(), LogTemp, Warning, TEXT("Unit spawned"));

	// Register this unit in the Units manager
	GetWorld()->GetSubsystem<UUnitsManagerSubsystem>()->RegisterUnit(this);
}

void ARTS_BaseUnit::AssignTask(UTask* InTask)
{
	if (!InTask)
		return;

	_currentTask = InTask;
	_currentTaskID = InTask->TaskID;
	UpdateBlackboardTask();
	InTask->OnTaskCompletedEvent.AddUObject(this, &ThisClass::Handle_TaskCompleted);
}

class UBehaviorTree* ARTS_BaseUnit::GetBTAsset() const
{
	if (BehaviorTreeAsset)
		return BehaviorTreeAsset.LoadSynchronous();
	return nullptr;
}

void ARTS_BaseUnit::Handle_TaskCompleted(UTask* Task)
{
	_currentTask = nullptr;
	_currentTaskID.Invalidate();
	UpdateBlackboardTask();

	// TODO: Broadcast OnUnitAvailableEvent
	OnUnitBecomeAvailable.Broadcast(this);
}

void ARTS_BaseUnit::UpdateBlackboardTask()
{
	if (AAIController* ai_controller = Cast<AAIController>(GetInstigatorController()))
	{
		if (UBlackboardComponent* blackboard = ai_controller->GetBlackboardComponent())
			blackboard->SetValueAsObject(NAME_CurrentTask, _currentTask);
	}
}

void ARTS_BaseUnit::LoadObjectData(FArchive& Ar)
{
	//Load assigned task for that unit
	if (_currentTaskID.IsValid())
	{
		if (UTask* task = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->GetTaskByID(_currentTaskID))
			AssignTask(task);
	}
}

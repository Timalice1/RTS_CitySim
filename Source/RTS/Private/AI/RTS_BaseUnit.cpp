#include "AI/RTS_BaseUnit.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/TasksManagerSubsystem.h"
#include "Managers/UnitsManagerSubsystem.h"
#include "Tasks/Task.h"
#include "Items/ItemActor.h"

DEFINE_LOG_CATEGORY(LogUnit);

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
	verify((_unitsManager = GetWorld()->GetSubsystem<UUnitsManagerSubsystem>()));

	_unitsManager->RegisterUnit(this);
	EnterIdleState();
}

void ARTS_BaseUnit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	int i = 10;
	for (const auto& item : _inventory) {
		UKismetSystemLibrary::DrawDebugString(
			GetWorld(),
			GetActorLocation() + FVector::UpVector * (300 + i),
			FString::Printf(TEXT("%s: %d"), *item.Key->GetName(), item.Value),
			nullptr, FLinearColor::Blue, 0
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

	if (UBlackboardComponent* blackboard = _controller->GetBlackboardComponent())
		blackboard->SetValueAsObject(NAME_CurrentTask, _currentTask);
	ExitIdleState();
}

bool ARTS_BaseUnit::IsPointReachable(const FVector& InTarget) const
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this->GetWorld());
	if (!NavSys) return false;
	const FPathFindingQuery query = FPathFindingQuery(this, NavSys->GetMainNavDataChecked(), this->GetActorLocation(),
	                                                  InTarget);
	return NavSys->TestPathSync(query);
}

void ARTS_BaseUnit::EnterIdleState()
{
	// HERE CAN BE ADDED A CHECKS, LIKE SCHEDULE STATE, DANGER ETC.
	GetWorldTimerManager()
		.SetTimer(_idleNotificationTimer,
		          FTimerDelegate::CreateLambda([this]()
		          {
			          // OR HERE
			          _unitsManager->NotifyUnitAvailable(this);
		          }),
		          IdleNotificationInterval + FMath::FRandRange(-.5f, .5f),
		          true);
}

void ARTS_BaseUnit::ExitIdleState()
{
	GetWorldTimerManager().ClearTimer(_idleNotificationTimer);
	_unitsManager->NotifyUnitUnavailable(this);
}

void ARTS_BaseUnit::Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message)
{
	_currentTask = nullptr;
	_currentTaskID.Invalidate();
	if (UBlackboardComponent* blackboard = _controller->GetBlackboardComponent())
		blackboard->ClearValue(NAME_CurrentTask);
	EnterIdleState();
}

class UBehaviorTree* ARTS_BaseUnit::GetDefaultBTAsset() const
{
	if (BehaviorTreeAsset)
		return BehaviorTreeAsset.LoadSynchronous();
	return nullptr;
}

bool ARTS_BaseUnit::TryAddItem(const TSubclassOf<AItemActor>& InItem)
{
	if (_currentItemsCount < MaxInventoryCapacity) {
		_inventory.FindOrAdd(InItem)++;
		_currentItemsCount++;
		return true;
	}
	return false;
}

uint8 ARTS_BaseUnit::GetItem(const TSubclassOf<AItemActor>& InItem) const
{
	if (const uint8* _items = _inventory.Find(InItem))
		return *_items;
	return 0;
}

void ARTS_BaseUnit::RemoveItem(const TSubclassOf<AItemActor>& InItem, const uint8& Amount)
{
	if (const uint8* _items = _inventory.Find(InItem)) {
		if (*_items == Amount)
			_inventory.Remove(InItem);
		else
			_inventory[InItem] = *_items - Amount;
		_currentItemsCount -= Amount;
	}
}

void ARTS_BaseUnit::LoadObjectData(FArchive& Ar)
{
	//Load and restore assigned task for that unit
	if (_currentTaskID.IsValid()) {
		if (UTask* task = GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->GetTaskByID(_currentTaskID))
			AssignTask(task);
	}
}

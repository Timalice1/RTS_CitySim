#include "Buildables/ConstructionProject.h"
#include "AIController.h"
#include "Buildables/BuildingData.h"
#include "Components/InventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/TasksManagerSubsystem.h"
#include "Materials/MaterialInstance.h"
#include "Tasks/ProgressiveTask.h"
#include "Tasks/Task_Collect.h"
#include "UI/Buildables/Building_InteractionPanel.h"
#include "UI/Generic/G_TextBlock.h"
#include "UI/Generic/InteractionPanelWidget.h"
#include "Items/ItemActor.h"

AConstructionProject::AConstructionProject()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Setup building mesh component
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	BuildingMesh->SetupAttachment(RootComponent);
	BuildingMesh->SetCollisionProfileName(TEXT("BlockAll"));

	InteractionPanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPanelWidgetComponent"));
	InteractionPanelWidget->SetupAttachment(BuildingMesh);
	InteractionPanelWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionPanelWidget->SetDrawAtDesiredSize(true);
}

void AConstructionProject::BeginPlay()
{
	Super::BeginPlay();
	InteractionPanelWidget->SetVisibility(false);
}

void AConstructionProject::Init(const FBuildingData& BuildingData)
{
	_buildingData = BuildingData;
	InitBuilding();

	// Apply preview validation overlay material to the mesh	
	ApplyOverlay();
}

void AConstructionProject::InitBuilding()
{
	if (!BuildingMesh)
		return;

	// Init mesh and mesh bounds
	if (UStaticMesh* meshComplete = _buildingData.BuildingMesh_Complete.LoadSynchronous()) {
		BuildingMesh->SetStaticMesh(meshComplete);

		// Init collider size
		FVector Min, Max;
		BuildingMesh->GetLocalBounds(Min, Max);
		buildingBounds = Min;

		// Building size adjustment to match world grid parameters
		// BuildingSize = MeshBounds + (CellSize - (MeshBounds % CellSize));
		const uint32 CellSize = _buildingData.CellSize;
		if ((static_cast<int>(Max.X) % CellSize != 0) && (static_cast<int>(Max.Y) % CellSize != 0)) {
			buildingBounds.X = Max.X + (CellSize - static_cast<int>(Max.X) % CellSize);
			buildingBounds.Y = Max.Y + (CellSize - static_cast<int>(Max.Y) % CellSize);
		}
	}

	// Initialize interaction panel widget
	if (const TSubclassOf<UInteractionPanelWidget> interactionPanelClass = InteractionPanelWidgetClass.
		LoadSynchronous()) {
		if (UBuilding_InteractionPanel* interactionPanel = Cast<UBuilding_InteractionPanel>(
			CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), interactionPanelClass))) {
			interactionPanel->Title->SetText(_buildingData.Title);
			interactionPanel->OnDestroyButtonClicked.BindUObject(this, &ThisClass::HandleCancelBuild);
			InteractionPanelWidget->SetWidget(interactionPanel);
		}
	}
}

void AConstructionProject::ApplyOverlay()
{
	// Create and apply dynamic overlay material instance
	if (UMaterialInstance* overlay = OverlayPreviewMaterial.LoadSynchronous()) {
		_overlayMaterial = UMaterialInstanceDynamic::Create(overlay, this);
		if (_overlayMaterial)
			BuildingMesh->SetOverlayMaterial(_overlayMaterial);
	}
}

void AConstructionProject::StartBuild()
{
	// TODO: Maybe replace with some construction fence or flags later
	BuildingMesh->SetStaticMesh(_buildingData.BuildingMesh_Stages[0].LoadSynchronous());

	// Disable overlay material	
	BuildingMesh->SetOverlayMaterial(nullptr);

	// Began satisfying requirements
	EvaluateState();
}

void AConstructionProject::EvaluateState()
{
	if (CheckResources()) {
		CreateBuildingTask();
		return;
	}

	for (const auto& required : _buildingData.RequiredResources) {
		int have = 0;

		if (const int* delivered = _deliveredResources.Find(required.Key)) have += *delivered;
		if (const int* pending = _pendingResources.Find(required.Key)) have += *pending;

		// TODO: FMath::CeilToInt(remaining / inventory capacity) -> tasks amount to create
		// TODO: limit tasks amount to available units count (optional)

		const int target = required.Value;
		if (have < target)
			CreateDeliveryTask(required.Key, target - have);
	}
}

void AConstructionProject::CreateDeliveryTask(const TSubclassOf<class AItemActor> InItemType,
                                              const int InTargetAmount)
{
	if (UTask_Collect* collectTask = NewObject<UTask_Collect>()) {
		collectTask->TaskID = FGuid::NewGuid();
		collectTask->ActionName = FName("Collecting");
		collectTask->TaskLocation = GetActorLocation();
		collectTask->TargetItemType = InItemType;
		collectTask->TargetAmount = InTargetAmount;

		collectTask->OnTaskFinished.AddDynamic(this, &ThisClass::Handle_ResourceDelivered);
		GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->RegisterTask(collectTask);

		_pendingResources.FindOrAdd(InItemType) += InTargetAmount;
	}
}

void AConstructionProject::CreateBuildingTask()
{
	//TODO: Replace with a UTask_Build : public UProgressiveTask
	if (UProgressiveTask* building_task = NewObject<UProgressiveTask>()) {
		building_task->TaskID = FGuid::NewGuid();
		building_task->ActionName = FName("Building");
		building_task->TaskLocation = GetActorLocation();
		building_task->WorkAmount = _buildingData.MaxDurability;

		building_task->OnProgressChanged.BindUObject(this, &AConstructionProject::UpdateBuildingProgress);
		building_task->OnTaskFinished.AddDynamic(this, &ThisClass::AConstructionProject::Handle_TaskFinished);
		GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->RegisterTask(building_task);
	}
}

bool AConstructionProject::CheckResources() const
{
	if (_deliveredResources.Num() != _buildingData.RequiredResources.Num())
		return false;
	for (const auto& target : _buildingData.RequiredResources) {
		const int* delivered = _deliveredResources.Find(target.Key);
		if (!delivered || *delivered < target.Value)
			return false;
	}
	return true;
}

void AConstructionProject::Handle_ResourceDelivered(UTask* Task, bool bSuccess, const FString& Message)
{
	const UTask_Collect* collectTask = Cast<UTask_Collect>(Task);
	if (!collectTask) return;

	const TSubclassOf<AItemActor> itemType = collectTask->TargetItemType;

	if (int* pending = _pendingResources.Find(itemType))
		*pending = (*pending > collectTask->TargetAmount) ? *pending - collectTask->TargetAmount : 0;

	if (bSuccess) {
		if (UInventoryComponent* inventory = Task->GetExecutor()->GetPawn()->GetComponentByClass<
			UInventoryComponent>()) {
			const int required = _buildingData.RequiredResources.FindRef(itemType);
			int remaining = required;
			if (const int* delivered = _deliveredResources.Find(itemType))
				remaining -= FMath::Min(*delivered, required); // in case if delivered > required

			const int toTake = FMath::Min(inventory->GetItem(itemType), remaining);
			_deliveredResources.FindOrAdd(itemType) += toTake;
			inventory->RemoveItem(itemType, toTake);
		}
	}

	EvaluateState();
}


void AConstructionProject::HandleCancelBuild()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
}

void AConstructionProject::UpdateBuildingProgress(const float InProgress) const
{
	if (!BuildingMesh)
		return;

	const int32 progressMeshIndex = FMath::Floor(InProgress * _buildingData.BuildingMesh_Stages.Num());
	if (_buildingData.BuildingMesh_Stages.IsValidIndex(progressMeshIndex)) {
		if (UStaticMesh* progressMesh = _buildingData.BuildingMesh_Stages[progressMeshIndex].LoadSynchronous())
			BuildingMesh->SetStaticMesh(progressMesh);
	}

	UKismetSystemLibrary::DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 1200.f,
	                                      FString::Printf(TEXT("Progress: {%.3f}"), InProgress), nullptr,
	                                      FLinearColor::Yellow, .01f);
}

void AConstructionProject::SetIsPlaceable(bool bIsPlaceable)
{
	if (_overlayMaterial)
		_overlayMaterial->SetScalarParameterValue(TEXT("Status"), bIsPlaceable ? 1.f : 0.f);
}

void AConstructionProject::SaveObjectData(FArchive& Ar) {}

void AConstructionProject::LoadObjectData(FArchive& Ar)
{
	InitBuilding();
	if (UProgressiveTask* task = Cast<UProgressiveTask>(
		GetWorld()->GetSubsystem<UTasksManagerSubsystem>()->GetTaskByID(_buildingTask))) {
		task->OnTaskFinished.AddDynamic(this, &ThisClass::Handle_TaskFinished);
		task->OnProgressChanged.BindUObject(this, &ThisClass::UpdateBuildingProgress);
	}
}

void AConstructionProject::Select()
{
	InteractionPanelWidget->SetVisibility(true);
}

void AConstructionProject::Deselect()
{
	InteractionPanelWidget->SetVisibility(false);
}

void AConstructionProject::Handle_TaskFinished(UTask* Task, bool bSuccess, const FString& message)
{
	OnBuildCompleted.Broadcast(this);
}

#include "Buildables/BuilderComponent.h"

#include "Buildables/Road.h"
#include "Buildables/RTS_BaseBuilding.h"
#include "Core/RTS_PlayerController.h"
#include "Buildables/RTS_BuildPreview.h"
#include "Core/Libraries/RTS_HUD_Library.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogBuilderComponent);

UBuilderComponent::UBuilderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBuilderComponent::BeginPlay()
{
	Super::BeginPlay();
	verify((player = Cast<ARTS_PlayerController>(GetOwner())) != nullptr);
	verify((WorldContext = GetWorld()) != nullptr);
	bIsBuildMode = false;
}

void UBuilderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!player)
		return;

	// Building preview actor placement
	if (bIsBuildMode && BuildPreviewActor)
	{
		FVector targetLocation = SnapToGrid(player->TraceMouseToLandscape());

		FVector intLocation = UKismetMathLibrary::VInterpTo(BuildPreviewActor->GetActorLocation(), targetLocation + FVector::UpVector * PreviewBuildFloatHeight, DeltaTime, PreviewBuildDragInterpSpeed);
		FRotator intRotation = UKismetMathLibrary::RInterpTo(BuildPreviewActor->GetActorRotation(), TargetRotation, DeltaTime, PreviewBuildRotationInterpSpeed);

		BuildPreviewActor->SetActorLocation(intLocation);
		BuildPreviewActor->SetActorRotation(intRotation);
		BuildPreviewActor->ValidatePlacement();
	}

	// Road tile placement
	if (bRoadBuildMode && RoadPreviewActor && !bPlaceRoad)
	{
		RoadPreviewActor->ClearPreview();
		const FVector targetLocation = SnapToGrid(player->TraceMouseToLandscape());
		RoadPreviewActor->AddPreviewInstance(targetLocation, FRotator::ZeroRotator, ValidateRoadTile(targetLocation));
	}
}

void UBuilderComponent::EnterBuildMode(const FBuildingData& BuildingData)
{
	if (!player || !WorldContext)
		return;

	if (bIsBuildMode && BuildPreviewActor)
	{
		// If already in build mode, destroy and remove previous building actor
		BuildPreviewActor->Destroy();
		BuildPreviewActor = nullptr;
	}

	// Try get and spawn new preview building actor
	if ((BuildPreviewActor = WorldContext->SpawnActor<ARTS_BuildPreview>(BuildPreview_Class.LoadSynchronous(), player->TraceMouseToLandscape(), FRotator::ZeroRotator)))
	{
		FBuildingData buildData = BuildingData;
		buildData.CellSize = CellSize;
		BuildPreviewActor->InitBuilding(buildData);
		OnBuildModeEnter.Broadcast();
		bIsBuildMode = true;

		if (bRoadBuildMode)
			ExitBuildMode();
	}
	else
	{
		UE_LOG(LogBuilderComponent, Warning, TEXT("EnterBuildMode: Failed to spawn preview building"));
		ExitBuildMode();
	}
}

void UBuilderComponent::RotateBuilding(const int32 direction)
{
	TargetRotation += FRotator(0, BuildingRotationStep, 0) * direction;
}

void UBuilderComponent::BuildDeploy()
{
	if (BuildPreviewActor && BuildPreviewActor->IsPlaceable())
	{
		if (ARTS_BuildPreview* build = WorldContext->SpawnActor<ARTS_BuildPreview>(
			BuildPreview_Class.LoadSynchronous(),
			BuildPreviewActor->GetActorLocation() - FVector::UpVector * PreviewBuildFloatHeight,
			BuildPreviewActor->GetActorRotation()))
		{
			build->InitBuilding(BuildPreviewActor->GetBuildingData());
			build->OnBuildCompleted.AddDynamic(this, &ThisClass::HandleBuildCompleted);
			build->StartBuild();
		}
		else
			UE_LOG(LogBuilderComponent, Warning, TEXT("BuildDeploy: Failed to spawn preview building"));
	}

	ExitBuildMode();
}

void UBuilderComponent::ExitBuildMode()
{
	if (bIsBuildMode)
	{
		bIsBuildMode = false;
		if (BuildPreviewActor)
		{
			BuildPreviewActor->Destroy();
			BuildPreviewActor = nullptr;
		}
		OnBuildModeExit.Broadcast();
	}

	if (bRoadBuildMode)
	{
		bRoadBuildMode = false;

		// Clean and destroy road preview
		if (RoadPreviewActor)
		{
			RoadPreviewActor->ClearPreview();
			RoadPreviewActor->Destroy();
			RoadPreviewActor = nullptr;
		}

		RoadPoints.Empty();
		OnRoadBuildExit.Broadcast();
	}
}

FVector UBuilderComponent::SnapToGrid(const FVector& WorldLocation) const
{
	FVector retVal = FVector();
	retVal.X = FMath::Floor(WorldLocation.X / CellSize) * CellSize;
	retVal.Y = FMath::Floor(WorldLocation.Y / CellSize) * CellSize;
	retVal.Z = WorldLocation.Z;
	return retVal;
}

void UBuilderComponent::HandleBuildCompleted(ARTS_BuildPreview* PreviewActor)
{
	check(PreviewActor);

	// Spawn actual building class
	FBuildingData buildData = PreviewActor->GetBuildingData();
	if (TSubclassOf<ARTS_BaseBuilding> buildingClass = buildData.Building_Class.LoadSynchronous())
	{
		if (ARTS_BaseBuilding* build = WorldContext->SpawnActor<ARTS_BaseBuilding>(
			buildingClass, PreviewActor->GetActorTransform()))
			build->Init(buildData);
	}

	URTS_HUD_Library::PushNotification(player, FString::Printf(TEXT("%s: Build completed"), *buildData.Title.ToString()), TEXT("Success"));

	PreviewActor->Destroy();
	OnBuildCompleted.Broadcast();
}

void UBuilderComponent::EnterRoadBuildMode()
{
	if (bRoadBuildMode)
	{
		ExitBuildMode();
		return;
	}

	bRoadBuildMode = true;

	verifyf((RoadPreviewActor = WorldContext->SpawnActor<ARoad_Preview>(RoadPreview_Class.LoadSynchronous())),
	        TEXT("Missing reference to RoadPreview actor class"));
	OnRoadBuildEnter.Broadcast();
	if (bIsBuildMode)
		ExitBuildMode();
}

void UBuilderComponent::Road_StartBuild()
{
	bPlaceRoad = true;
	// Cache road start position, snapped to the grid location
	Road_StartPosition = SnapToGrid(player->TraceMouseToLandscape());
}

void UBuilderComponent::Road_Build()
{
	if (!bPlaceRoad)
		return;

	RoadPoints.Empty();
	if (RoadPreviewActor)
		RoadPreviewActor->ClearPreview();

	const FVector MouseCurrent = SnapToGrid(player->TraceMouseToLandscape());
	const FVector roadDirection = MouseCurrent - Road_StartPosition;

	// Find bounding box size (manhattan)
	const FVector boxSize = FVector(FMath::Abs(Road_StartPosition.X - MouseCurrent.X), FMath::Abs(Road_StartPosition.Y - MouseCurrent.Y), 0.f);

	// If perimeter == 1, that means that bounding box are 1x1 cells size, so we can recalculate primary axis direction
	// Otherwise, primary axis direction is locked
	if (FMath::Floor(boxSize.X / CellSize) + FMath::Floor(boxSize.Y / CellSize) == 1)
		primaryAxisDirection = FMath::Abs(roadDirection.X) > FMath::Abs(roadDirection.Y) ? FVector::ForwardVector : FVector::RightVector;

	// Actual primary axis direction with signs
	const FVector _primaryDir = primaryAxisDirection * FMath::Sign(FVector::DotProduct(roadDirection, primaryAxisDirection));
	const int32 primaryAxisLenght = FMath::Floor((_primaryDir * boxSize).Length() / CellSize);
	CreateRoadPreviewTiles(Road_StartPosition, _primaryDir, primaryAxisLenght);

	// Calculate secondary axis
	const FVector cornerTileLocation = SnapToGrid(((primaryAxisLenght * CellSize) * _primaryDir) + Road_StartPosition); // Last point location from primary axis
	FVector secondaryAxisDirection = (MouseCurrent - cornerTileLocation).GetSafeNormal2D();
	secondaryAxisDirection.X = FMath::RoundToInt(secondaryAxisDirection.X);
	secondaryAxisDirection.Y = FMath::RoundToInt(secondaryAxisDirection.Y);
	const int32 secondaryAxisLenght = FMath::Floor((secondaryAxisDirection * boxSize).Length() / CellSize);
	CreateRoadPreviewTiles(cornerTileLocation, secondaryAxisDirection, secondaryAxisLenght + 1); // +1 for the last tile on cursor location
}

void UBuilderComponent::Road_Deploy()
{
	// If road is not canceled
	// Create new Road actor from loaded class
	// For each calculated stored point, check point validation
	// If point is valid -> add new instance to Road actor ISM Component on target position
	if (!bPlaceRoad)
		return;

	if (RoadPreviewActor)
		RoadPreviewActor->ClearPreview();

	// Spawn actual road actor instead
	if (ARoad* road = WorldContext->SpawnActor<ARoad>(Road_Class.LoadSynchronous(), Road_StartPosition, FRotator::ZeroRotator))
	{
		for (FRoadCell& cell : RoadPoints)
		{
			if (cell.bIsValid)
				road->AddInstance(cell.Location, cell.Rotation);
		}
	}
	else
		UE_LOG(LogBuilderComponent, Warning, TEXT("Failed to spawn road actor from loaded class"));

	bPlaceRoad = false;
}

void UBuilderComponent::Road_BuildCancel()
{
	bPlaceRoad = false;
	RoadPoints.Empty();

	if (RoadPreviewActor)
		RoadPreviewActor->ClearPreview();

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Cancel build road")));
}

bool UBuilderComponent::ValidateRoadTile(const FVector& InLocation) const
{
	// All meshes, actors and any collision use Visibility channel as a default blocking channel
	// Visibility channel are only ignored by terrain meshes, who use custom trace channel
	// So, point will be only valid on the terrain, preventing any overlaps with another meshes and actors
	return !WorldContext->OverlapAnyTestByChannel(InLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(FVector(CellSize * .4f)));
}

void UBuilderComponent::CreateRoadPreviewTiles(const FVector& Start, const FVector& Direction, const int32 Size)
{
	for (int32 i = 0; i < Size; i++)
	{
		const FVector targetLocation = Start + Direction * (i * CellSize);
		const FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(targetLocation, targetLocation + Direction * CellSize);
		const bool IsValidPoint = ValidateRoadTile(targetLocation);

		RoadPoints.Add(FRoadCell(targetLocation, targetRotation, IsValidPoint));
		if (RoadPreviewActor)
			RoadPreviewActor->AddPreviewInstance(targetLocation, targetRotation, IsValidPoint);
	}
}

void UBuilderComponent::LoadObjectData(FArchive& Ar)
{
	// Rebinding preview buildings completed delegates
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTS_BuildPreview::StaticClass(), outActors);
	for (AActor* actor : outActors)
	{
		if (ARTS_BuildPreview* build = Cast<ARTS_BuildPreview>(actor))
			build->OnBuildCompleted.AddDynamic(this, &ThisClass::HandleBuildCompleted);
	}
}

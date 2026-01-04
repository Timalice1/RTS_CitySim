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
		BuildPreviewActor->SetIsPlaceable(ValidatePlacement(intLocation, BuildPreviewActor->GetBuildingBounds(), BuildPreviewActor));
	}

	// Road tile placement
	if (bRoadBuildMode && RoadPreviewActor && !bPlaceRoad)
	{
		RoadPreviewActor->ClearPreview();
		const FVector targetLocation = SnapToGrid(player->TraceMouseToLandscape());
		RoadPreviewActor->AddPreviewInstance(targetLocation, FRotator::ZeroRotator, ValidatePlacement(targetLocation, FVector(CellSize * .4, CellSize * .4, .5f)));
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
	if (BuildPreviewActor && ValidatePlacement(BuildPreviewActor->GetActorLocation(), BuildPreviewActor->GetBuildingBounds(), BuildPreviewActor))
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
	retVal.Z = 0;
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
	const FVector boxSize = FVector(FMath::Abs(Road_StartPosition.X - MouseCurrent.X),
	                                FMath::Abs(Road_StartPosition.Y - MouseCurrent.Y),
	                                FMath::Abs(Road_StartPosition.Z - MouseCurrent.Z));

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

	if (!IsRoadValid())
	{
		bPlaceRoad = false;
		return;
	}

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
}

bool UBuilderComponent::ValidatePlacement(const FVector& InLocation, const FVector& InObjectSize, AActor* IgnoredActor) const
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(IgnoredActor);

	const bool bPlaceable = !WorldContext->OverlapAnyTestByChannel(InLocation + FVector::UpVector, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(InObjectSize), queryParams);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), InLocation, InObjectSize, FLinearColor::Blue, FRotator::ZeroRotator, 0, 1.f);

	return bPlaceable;
}

void UBuilderComponent::CreateRoadPreviewTiles(const FVector& Start, const FVector& Direction, const int32 Size)
{
	for (int32 i = 0; i < Size; i++)
	{
		FVector targetLocation = Start + Direction * (i * CellSize);
		FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(targetLocation, targetLocation + Direction * CellSize);
		const bool IsValidPoint = ValidatePlacement(targetLocation, FVector(CellSize * .4f, CellSize * .4f, .5f));

		RoadPoints.Add(FRoadCell(targetLocation, targetRotation, IsValidPoint));
		if (RoadPreviewActor)
			RoadPreviewActor->AddPreviewInstance(targetLocation, targetRotation, IsValidPoint);
	}
}

bool UBuilderComponent::IsRoadValid()
{
	int32 ValidTiles = 0;
	for (const FRoadCell& Cell : RoadPoints)
	{
		if (Cell.bIsValid)
			ValidTiles++;
	}
	return ValidTiles > 0;
}

void UBuilderComponent::LoadObjectData(FArchive& Ar)
{
	// Rebinding preview buildings completed delegates
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTS_BuildPreview::StaticClass(), outActors);
	for (AActor* actor : outActors)
	{
		if (ARTS_BuildPreview* build = Cast<ARTS_BuildPreview>(actor))
		{
			build->OnBuildCompleted.RemoveAll(this);
			build->OnBuildCompleted.AddDynamic(this, &ThisClass::HandleBuildCompleted);
		}
	}
}

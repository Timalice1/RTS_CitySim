#include "AI/RTS_BasePawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/UnitsManagerSubsystem.h"

ARTS_BasePawn::ARTS_BasePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseControllerRotationYaw = true;

	// Setup mesh component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	BodyMesh->SetupAttachment(RootComponent);

	// Setup movement component
	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
}

void ARTS_BasePawn::BeginPlay()
{
	Super::BeginPlay();

	// Register this unit in the Units manager
	GetWorld()->GetSubsystem<UUnitsManagerSubsystem>()->RegisterUnit(this);
	_currentStatus = FMath::RandBool() ? Idle : Work; // TEMP for testing purposes
}

void ARTS_BasePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UKismetSystemLibrary::DrawDebugString(GetWorld(),
	                                      GetActorLocation() + FVector::UpVector * 250,
	                                      UEnum::GetValueAsString(_currentStatus), 0,
	                                      _currentStatus == Idle ? FLinearColor::Yellow : FLinearColor::Green,
	                                      .1f);
}

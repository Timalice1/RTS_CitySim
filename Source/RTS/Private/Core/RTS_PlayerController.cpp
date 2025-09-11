#include "Core/RTS_PlayerController.h"
#include "Core/RTS_CameraPawn.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RTS_HUD.h"
#include "Buildables/BuilderComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRTSController, Log, All)

ARTS_PlayerController::ARTS_PlayerController()
{
	BuilderComp = CreateDefaultSubobject<UBuilderComponent>(TEXT("Builder"));
}

void ARTS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Cache reference to a HUD class
	verify((RTS_HUD = Cast<ARTS_HUD>(GetHUD())) != nullptr);

	if (BuilderComp)
	{
		BuilderComp->OnBuildModeEnter.AddDynamic(this, &ThisClass::HandleBuildModeEnter);
		BuilderComp->OnBuildModeExit.AddDynamic(this, &ThisClass::HandleBuildModeExit);
		BuilderComp->OnRoadBuildEnter.AddDynamic(this, &ThisClass::HandleRoadBuildEnter);
		BuilderComp->OnRoadBuildExit.AddDynamic(this, &ThisClass::HandleBuildRoadExit);
	}

	// UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("stat unit"), this);
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("stat fps"), this);
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("t.MaxFPS 200"), this);
}

void ARTS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Cache references to a controlled pawn and InputSubsystem
	verify((_controlledPawn = Cast<ARTS_CameraPawn>(InPawn)) != nullptr);
	verify((InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) != nullptr);

	// Setup default input mapping context
	InputSystem->AddMappingContext(DefaultInputMapping.LoadSynchronous(), 0);
}

#pragma region InputListeners

void ARTS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Input binding
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(InputActions.ESC, ETriggerEvent::Triggered, this, &ThisClass::ESC_Trigger);

		// Camera actions mappings
		Input->BindAction(InputActions.MoveCamera, ETriggerEvent::Triggered, this, &ThisClass::MoveCamera);
		Input->BindAction(InputActions.RotateCamera, ETriggerEvent::Triggered, this, &ThisClass::RotateCamera);
		Input->BindAction(InputActions.HoldRotate, ETriggerEvent::Triggered, this, &ThisClass::StartRotateCamera);
		Input->BindAction(InputActions.HoldRotate, ETriggerEvent::Completed, this, &ThisClass::StopRotateCamera);
		Input->BindAction(InputActions.ZoomCamera, ETriggerEvent::Triggered, this, &ThisClass::ZoomCamera);

		// Buildings system inputs
		Input->BindAction(InputActions.BuildDeploy, ETriggerEvent::Started, this, &ThisClass::BuildDeploy);
		Input->BindAction(InputActions.CancelBuild, ETriggerEvent::Started, this, &ThisClass::BuildCancel);
		Input->BindAction(InputActions.RotateBuilding, ETriggerEvent::Triggered, this, &ThisClass::RotateBuilding);

		Input->BindAction(InputActions.BuildRoad, ETriggerEvent::Started, this, &ThisClass::BuildRoadStart);
		Input->BindAction(InputActions.BuildRoad, ETriggerEvent::Triggered, this, &ThisClass::BuildRoadPending);
		Input->BindAction(InputActions.BuildRoad, ETriggerEvent::Completed, this, &ThisClass::BuildRoadCompleted);
		Input->BindAction(InputActions.CancelBuildRoad, ETriggerEvent::Started, this, &ThisClass::BuildRoadCancel);
	}
}

void ARTS_PlayerController::ESC_Trigger(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp->IsBuildModeEnabled())
	{
		BuilderComp->ExitBuildMode();
		return;
	}

	if (RTS_HUD->CloseActivePanel())
		return;

	RTS_HUD->TogglePauseMenu();
}

void ARTS_PlayerController::MoveCamera(const FInputActionInstance& ActionInstance)
{
	FVector2D axisValue = ActionInstance.GetValue().Get<FVector2D>();
	_controlledPawn->Move(axisValue);
}

void ARTS_PlayerController::StartRotateCamera(const FInputActionInstance& ActionInstance)
{
	bCameraRotationEnabled = true;
}

void ARTS_PlayerController::StopRotateCamera(const FInputActionInstance& ActionInstance)
{
	bCameraRotationEnabled = false;
}

void ARTS_PlayerController::RotateCamera(const FInputActionInstance& ActionInstance)
{
	if (!bCameraRotationEnabled)
		return;
	_controlledPawn->RotateCamera(ActionInstance.GetValue().Get<float>());
}

void ARTS_PlayerController::ZoomCamera(const FInputActionInstance& ActionInstance)
{
	if (bCameraZoomEnabled)
		_controlledPawn->Zoom(ActionInstance.GetValue().Get<float>());
}

void ARTS_PlayerController::BuildDeploy(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp->IsBuildModeEnabled())
		BuilderComp->BuildDeploy();
}

void ARTS_PlayerController::BuildCancel(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp->IsBuildModeEnabled())
		BuilderComp->ExitBuildMode();
}

void ARTS_PlayerController::RotateBuilding(const FInputActionInstance& ActionInstance)
{
	ensure(BuilderComp);
	float rotationDirection = ActionInstance.GetValue().Get<float>();
	BuilderComp->RotateBuilding(static_cast<int>(rotationDirection));
}

void ARTS_PlayerController::BuildRoadStart(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp)
		BuilderComp->Road_StartBuild();
}

void ARTS_PlayerController::BuildRoadPending(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp)
		BuilderComp->Road_Build();
}

void ARTS_PlayerController::BuildRoadCancel(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp)
		BuilderComp->Road_BuildCancel();
}

void ARTS_PlayerController::BuildRoadCompleted(const FInputActionInstance& ActionInstance)
{
	if (BuilderComp)
		BuilderComp->Road_Deploy();
}

#pragma endregion InputListeners

void ARTS_PlayerController::HandleBuildModeEnter()
{
	RTS_HUD->ToggleBuildingsPanel();
	InputSystem->AddMappingContext(BuildingsInputMapping.LoadSynchronous(), 1);
}

void ARTS_PlayerController::HandleBuildModeExit()
{
	InputSystem->RemoveMappingContext(BuildingsInputMapping.LoadSynchronous());
}

void ARTS_PlayerController::HandleRoadBuildEnter()
{
	RTS_HUD->ToggleBuildingsPanel();
	InputSystem->AddMappingContext(RoadsInputMapping.LoadSynchronous(), 2);
}

void ARTS_PlayerController::HandleBuildRoadExit()
{
	InputSystem->RemoveMappingContext(RoadsInputMapping.LoadSynchronous());
}

AActor* ARTS_PlayerController::GetActorUnderCursor()
{
	FHitResult hit;

	if (GetHitResultUnderCursorByChannel(InteractibleTraceChannel, false, hit))
	{
		if (AActor* hitActor = hit.GetActor())
			return hitActor;
	}
	return nullptr;
}

FVector ARTS_PlayerController::TraceMouseToLandscape() const
{
	FHitResult hit;
	FVector HitLocation = FVector::ZeroVector;
	if (GetHitResultUnderCursorByChannel(LandscapeTraceChannel, false, hit))
		HitLocation = hit.Location;
	return HitLocation;
}

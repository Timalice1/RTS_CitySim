#include "Core/RTS_CameraPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet\KismetMathLibrary.h"

ARTS_CameraPawn::ARTS_CameraPawn()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    Camera->SetupAttachment(SpringArm);

    PawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovementComponent");
}

void ARTS_CameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Initial arm lenght
    float initialHeight = (MinHeight + MaxHeight) / 2;
    SpringArm->SocketOffset = FVector(0, 0, 1) * initialHeight;

    // Initial camera tilt
    FRotator currentRotation = Camera->GetRelativeRotation();
    currentRotation.Pitch = UKismetMathLibrary::MapRangeClamped(initialHeight, MinHeight, MaxHeight, TiltMin, TiltMax);
    Camera->SetRelativeRotation(currentRotation);
}

void ARTS_CameraPawn::Move(const FVector2D Axis)
{
    FVector forwardVector = GetActorForwardVector();
    FVector rightVector = GetActorRightVector();
    this->AddMovementInput(forwardVector, Axis.X);
    this->AddMovementInput(rightVector, Axis.Y);
}

void ARTS_CameraPawn::RotateCamera(const float Val)
{
    FRotator currentRotation = GetActorRotation();
    currentRotation.Yaw += Val * CamRotationSpeed;
    SetActorRotation(currentRotation);
}

void ARTS_CameraPawn::Zoom(const float Val)
{
    float currentHeight = SpringArm->SocketOffset.Z;
    currentHeight += Val * ZoomSpeed;
    currentHeight = FMath::Clamp(currentHeight, MinHeight, MaxHeight);
    SpringArm->SocketOffset = FVector(0, 0, 1) * currentHeight;

    // camera tilt
    FRotator currentRotation = Camera->GetRelativeRotation();
    currentRotation.Pitch = UKismetMathLibrary::MapRangeClamped(currentHeight, MinHeight, MaxHeight, TiltMin, TiltMax);
    Camera->SetRelativeRotation(currentRotation);
}
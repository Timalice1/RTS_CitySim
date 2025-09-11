#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTS_CameraPawn.generated.h"

UCLASS()
class RTS_API ARTS_CameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ARTS_CameraPawn();
    virtual void BeginPlay() override;

protected: // Components
    UPROPERTY(EditDefaultsOnly, Category = Components)
    TObjectPtr<class UCameraComponent> Camera = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = Components)
    TObjectPtr<class USpringArmComponent> SpringArm = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = Components)
    TObjectPtr<class UFloatingPawnMovement> PawnMovementComponent = nullptr;

protected: // Parameters
    UPROPERTY(EditDefaultsOnly, Category = PawnCfg)
    float CamRotationSpeed = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "PawnCfg|Zoom")
    float ZoomSpeed = 20.f;
    UPROPERTY(EditDefaultsOnly, Category = "PawnCfg|Zoom")
    float MinHeight = 200.f;
    UPROPERTY(EditDefaultsOnly, Category = "PawnCfg|Zoom")
    float MaxHeight = 3000.f;
    UPROPERTY(EditDefaultsOnly, Category = "PawnCfg|Zoom")
    float TiltMin = -90.f;
    UPROPERTY(EditDefaultsOnly, Category = "PawnCfg|Zoom")
    float TiltMax = 0.f;

public:
    /// Move camera pawn
    void Move(const FVector2D Axis);
    /// Rotate the camera pawn around Yaw-axis
    virtual void RotateCamera(const float Val);
    /// Zoom in/out and tilt camera
    virtual void Zoom(const float Val);
};
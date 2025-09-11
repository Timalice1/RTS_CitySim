#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActions.h"
#include "RTS_PlayerController.generated.h"

class APawn;
class AActor;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS()
class RTS_API ARTS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARTS_PlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/* Returns clicked actor under cursor*/
	UFUNCTION(BlueprintCallable, Category = "RTS_PlayerController|Misc")
	AActor* GetActorUnderCursor();

	/* Perform a single line trace, using custom Landscape channel. Return hit location res*/
	UFUNCTION(BlueprintCallable, Category = "RTS_PlayerController|Misc")
	FVector TraceMouseToLandscape() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UBuilderComponent> BuilderComp;

	/*Default trace channel reference*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|Trace")
	TEnumAsByte<ETraceTypeQuery> DefaultTraceChannel;

	/*Response trace channel for interactable actors*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|Trace")
	TEnumAsByte<ETraceTypeQuery> InteractibleTraceChannel;

	/*Custom trace channel for landscape*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|Trace")
	TEnumAsByte<ETraceTypeQuery> LandscapeTraceChannel;

	/*Default mapping context reference*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|EnhancedInput|Mappings")
	TSoftObjectPtr<UInputMappingContext> DefaultInputMapping;

	/*Buildings system mapping context reference*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|EnhancedInput|Mappings")
	TSoftObjectPtr<UInputMappingContext> BuildingsInputMapping;

	/* Road placement mapping context*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|EnhancedInput|Mappings")
	TSoftObjectPtr<UInputMappingContext> RoadsInputMapping;
	
	/*List of action inputs*/
	UPROPERTY(EditDefaultsOnly, Category = "Config|EnhancedInput|Actions")
	FInputActions InputActions;

private: // Input actions listeners
	virtual void SetupInputComponent() override;

	void ESC_Trigger(const FInputActionInstance& ActionInstance);
	
	/* Camera input listeners*/
	void MoveCamera(const FInputActionInstance& ActionInstance);
	void ZoomCamera(const FInputActionInstance& ActionInstance);
	void RotateCamera(const FInputActionInstance& ActionInstance);
	void StartRotateCamera(const FInputActionInstance& ActionInstance);
	void StopRotateCamera(const FInputActionInstance& ActionInstance);

	/*Buildings system input listeners*/
	void BuildDeploy(const FInputActionInstance& ActionInstance);
	void BuildCancel(const FInputActionInstance& ActionInstance);
	void RotateBuilding(const FInputActionInstance& ActionInstance);

	void BuildRoadStart(const FInputActionInstance& ActionInstance);
	void BuildRoadPending(const FInputActionInstance& ActionInstance);
	void BuildRoadCompleted(const FInputActionInstance& ActionInstance);
	void BuildRoadCancel(const FInputActionInstance& ActionInstance);

protected: // Internal

	/*Callback from BuilderComponent*/
	UFUNCTION()
	virtual void HandleBuildModeEnter();
	
	/*Callback from Builder component*/
	UFUNCTION()
	virtual void HandleBuildModeExit();

	UFUNCTION()
	virtual void HandleRoadBuildEnter();

	UFUNCTION()
	virtual void HandleBuildRoadExit();

#pragma region Internal stuff

private:
	UPROPERTY() // Cached controlled pawn reference
	TObjectPtr<class ARTS_CameraPawn> _controlledPawn;
	UPROPERTY() // Cached RTS_HUD Class reference
	TObjectPtr<class ARTS_HUD> RTS_HUD;
	UPROPERTY() // Cached input subsystem reference
	UEnhancedInputLocalPlayerSubsystem* InputSystem = nullptr;

	/* Is mouse dragging? */
	bool bIsDragging = false;
	/* Can camera zoom in/out*/
	bool bCameraZoomEnabled = true;
	/* Should camera rotate? */
	bool bCameraRotationEnabled = false;

#pragma endregion
};

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActions.generated.h"

class UInputAction;


/**
 * Action inputs list structure.
 * Reference to this structure was attached into the player controller class.	
 * Defines a list of required input actions, that will be used across the game. 
*/
USTRUCT()
struct FInputActions
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Default)
	TObjectPtr<UInputAction> ESC = nullptr;
		
	// Camera pawn input actions
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<UInputAction> MoveCamera = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<UInputAction> RotateCamera = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<UInputAction> HoldRotate = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<UInputAction> ZoomCamera = nullptr;

	// Buildings system input actions
	UPROPERTY(EditDefaultsOnly, Category = BuildingsSystem)
	TObjectPtr<UInputAction> BuildDeploy = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = BuildingsSystem)
	TObjectPtr<UInputAction> CancelBuild = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = BuildingsSystem)
	TObjectPtr<UInputAction> RotateBuilding = nullptr;

	// Roads placement input actions
	UPROPERTY(EditDefaultsOnly, Category = "BuildingsSystem|Roads")
	TObjectPtr<UInputAction> BuildRoad = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "BuildingsSystem|Roads")
	TObjectPtr<UInputAction> CancelBuildRoad = nullptr;
};

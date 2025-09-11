#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTS_HUD.generated.h"

class AActor;
class APlayerController;
class UL_GameMenus;
class UL_Modals;
class UL_Menus;

DECLARE_LOG_CATEGORY_EXTERN(LogHUD, Log, All);

UCLASS()
class RTS_API ARTS_HUD : public AHUD
{
	GENERATED_BODY()

public: // Native events
	virtual void BeginPlay() override;

protected: // HUD Properties
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	APlayerController* owningPlayer = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Layers)
	TSoftClassPtr<UL_GameMenus> GameMenusLayer_Class;

	UPROPERTY(EditDefaultsOnly, Category = Layers)
	TSoftClassPtr<UL_Modals> ModalsLayer_Class;

	UPROPERTY(EditDefaultsOnly, Category = Layers)
	TSoftClassPtr<UL_Menus> MenusLayer_Class;
	
public:
	/// Return a reference to modals layer widget
	UFUNCTION(BlueprintCallable, Category = "RTS_HUD|Layers")
	virtual UL_Modals* ModalsLayer() { return ModalsLayer_Widget; }

	/// Redirect to Game menus layer API
	/// Toggle Buildings menu panel widget
	UFUNCTION(BlueprintCallable, Category = "RTS_HUD|Layers|GameMenusLayer")
	virtual void ToggleBuildingsPanel();

	/// Redirect to Game menus layer API
	/// Close active in-game menu panel
	UFUNCTION(BlueprintCallable, Category = "RTS_HUD|Layers|GameMenusLayer")
	virtual bool CloseActivePanel();

	UFUNCTION(BlueprintCallable, Category = "RTS_HUD|Layers")
	virtual void TogglePauseMenu();
	
private: // Internal
	virtual void InitLayers();

	bool bPauseMenuOpened = false;
	
	//================= Layers ========================
	UPROPERTY()
	TArray<class UUserWidget*> UILayers;
	
	UPROPERTY()
	TObjectPtr<UL_GameMenus> GameMenusLayer_Widget = nullptr;
	UPROPERTY()
	TObjectPtr<UL_Modals> ModalsLayer_Widget = nullptr;
	UPROPERTY()
	TObjectPtr<UL_Menus> MenusLayer_Widget = nullptr;
	//=================================================

	
};

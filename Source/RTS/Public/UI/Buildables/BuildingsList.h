#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingsList.generated.h"

struct FBuildingData;

UCLASS(Abstract)
class RTS_API UBuildingsList : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ItemsList = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Confing)
	TSoftClassPtr<class UBuildItemWidget> BuildItem_Class;

	UPROPERTY(EditDefaultsOnly, Category = Confing)
	TSoftObjectPtr<class UDataTable> BuildingsTable;

private:
	UFUNCTION()
	void OnBuildItemSelected(const FBuildingData& BuildData);
};

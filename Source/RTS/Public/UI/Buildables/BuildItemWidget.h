#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Buildables/BuildingData.h"
#include "BuildItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildItemSelectedEvent, const FBuildingData&, BuildData);

UCLASS(Abstract)
class RTS_API UBuildItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	virtual void Init(const FBuildingData& BuildData);

	UFUNCTION(BlueprintCallable, Category = BuildItemWidget)
	virtual FBuildingData GetBuildData() { return _buildData; }

	UFUNCTION()
	virtual void HandleButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ItemButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UG_TextBlock> ItemTitle = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemIcon = nullptr;

public:
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnBuildItemSelectedEvent OnBuildItemSelected;

private:
	FBuildingData _buildData;
};

#include "UI/Buildables/BuildingsList.h"

#include "Buildables/BuilderComponent.h"
#include "UI/Buildables/BuildItemWidget.h"
#include "Buildables/BuildingData.h"
#include "Components/ScrollBox.h"
#include "Engine/DataTable.h"

void UBuildingsList::NativeConstruct()
{
	Super::NativeConstruct();

	// Clear current list
	ItemsList->ClearChildren();

	// Populate items list
	TArray<FBuildingData*> outRows;
	if (UDataTable* table = BuildingsTable.LoadSynchronous())
		table->GetAllRows<FBuildingData>(FString(), outRows);

	if (!outRows.IsEmpty())
	{
		if (TSubclassOf<UBuildItemWidget> buildItemClass = BuildItem_Class.LoadSynchronous())
		{
			for (FBuildingData* buildData : outRows)
			{
				// Create new item widget
				if (UBuildItemWidget* buildWidget = CreateWidget<UBuildItemWidget>(this, buildItemClass))
				{
					buildWidget->Init(*buildData);
					buildWidget->OnBuildItemSelected.AddDynamic(this, &ThisClass::OnBuildItemSelected);
					ItemsList->AddChild(buildWidget);
				}
			}
		}
	}
}

void UBuildingsList::OnBuildItemSelected(const FBuildingData& BuildData)
{
	if (UBuilderComponent* builder = GetOwningPlayer()->FindComponentByClass<UBuilderComponent>())
		builder->EnterBuildMode(BuildData);
}

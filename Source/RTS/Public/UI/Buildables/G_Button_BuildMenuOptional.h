#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G_Button_BuildMenuOptional.generated.h"

/**
 * Optional button, that bounded to building system but are not Building Item
 * Can be road tool, fence/wall tool or demolition tool
 *
 * TODO: Add reference here to BuilderComponent
 */
UCLASS(Abstract)
class RTS_API UG_Button_BuildMenuOptional : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedEvent);

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnClickedEvent OnClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ButtonIcon = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
	TSoftObjectPtr<class UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere, Category = Config)
	FVector2D Size = FVector2D(32.f, 32.f);

	UFUNCTION(BlueprintCallable, Category = BuildMenuButton_Optional)
	class UBuilderComponent* GetBuilderComponent() const { return BuilderComponent; }

private:
	UFUNCTION()
	void HandleClicked();
	UPROPERTY()
	TObjectPtr<class UBuilderComponent> BuilderComponent = nullptr;
};

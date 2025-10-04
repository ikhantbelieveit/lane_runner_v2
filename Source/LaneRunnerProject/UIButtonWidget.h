// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class LANERUNNERPROJECT_API UUIButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Editable label text (visible in editor + Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button")
	FText LabelText;

	// Editable arrow texture (visible in editor + Blueprint)
	// You can drop a Texture2D in the UMG asset's details to set the arrow image.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button")
	UTexture2D* ArrowTexture;

protected:
	// Bind to named widgets inside the UMG designer using these exact names or change meta names accordingly.
	UPROPERTY(meta = (BindWidget))
	UButton* RootButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LabelTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* ArrowImage;

	// UUserWidget override points
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

public:
	// Convenient setters (callable from C++/Blueprint)
	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void SetLabelText(const FText& NewText);

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void SetArrowTexture(UTexture2D* NewTexture);

	// Show/hide arrow explicitly
	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void ShowArrow(bool bShow);
};

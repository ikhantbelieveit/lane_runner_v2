// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIButtonEvent);
UCLASS()
class LANERUNNERPROJECT_API UUIButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button")
	FText LabelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button")
	UTexture2D* ArrowTexture;

protected:

	UPROPERTY(meta = (BindWidget))
	UButton* RootButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LabelTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* ArrowImage;


	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

public:

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void SetLabelText(const FText& NewText);

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void SetArrowTexture(UTexture2D* NewTexture);


	UFUNCTION(BlueprintCallable, Category = "Custom Button")
	void ShowArrow(bool bShow);

	UFUNCTION()
	void OnRootButtonClick();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FUIButtonEvent BroadcastButtonClick;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FUIButtonEvent BroadcastFocusGain;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FUIButtonEvent BroadcastFocusLost;
};

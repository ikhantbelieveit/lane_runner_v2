// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/Image.h" 
#include "Components/TextBlock.h"
#include "UIButtonWidget.h"
#include "MainMenuUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UMainMenuUIWidget : public UBaseUIScreen
{
	GENERATED_BODY()

	virtual void Initialise() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnScreenShown() override;



public:
	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* StartButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* PlayButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* QuitButton;

	UFUNCTION()
	void OnStartButtonPressed();

	UFUNCTION()
	void OnPlayButtonPressed();

	UFUNCTION()
	void OnQuitButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VersionNumberDisplay;
};

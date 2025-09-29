// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/Image.h" 
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

public:
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void OnStartButtonPressed();

	UFUNCTION()
	void OnPlayButtonPressed();

	UFUNCTION()
	void OnQuitButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UImage* StartButtonArrow;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayButtonArrow;

	UPROPERTY(meta = (BindWidget))
	UImage* QuitButtonArrow;
};

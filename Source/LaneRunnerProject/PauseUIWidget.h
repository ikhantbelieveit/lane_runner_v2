// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIButtonWidget.h"
#include "BaseUIScreen.h"
#include "PauseUIWidget.generated.h"


/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UPauseUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* RestartButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* QuitToMenuButton;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* QuitToDesktopButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScreenInputMode PauseInputMode = EScreenInputMode::None;

	UFUNCTION()
	void Initialise();

	UFUNCTION()
	void OnResumeButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnRestartButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnSettingsButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnQuitToMenuButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnQuitToDesktopButtonPressed(UUIButtonWidget* button);


};

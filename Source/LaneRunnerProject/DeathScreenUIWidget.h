// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "DeathScreenUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UDeathScreenUIWidget : public UBaseUIScreen
{
	GENERATED_BODY()
	
public:
	virtual void Initialise() override;

	UPROPERTY(meta = (BindWidget))
	UButton* RespawnButton;

	UFUNCTION()
	void OnRespawnButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void OnQuitButtonPressed();

	virtual void SetupBeforeShow() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HighScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewHighScoreMessageText;

	void SetScoreText(int newScore);

	void SetHighScoreText(int newHighScore);

	void SetMessageActive(bool active);
};

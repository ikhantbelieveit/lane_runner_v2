// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UIButtonWidget.h"
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

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* RespawnButton;

	UFUNCTION()
	void OnRespawnButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* QuitButton;

	UFUNCTION()
	void OnQuitButtonPressed();

	virtual void OnScreenShown() override;
	virtual void OnScreenHidden() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BaseScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DistanceValueText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DistanceBonusText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HighScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewHighScoreMessageText;

	void SetBasePointsText(int basePoints);
	void SetDistanceValueText(int distance);
	void SetDistanceBonusText(int bonusVal);
	void SetScoreTotalText(int pointsTotal);

	void SetHighScoreText(int newHighScore);

	void SetMessageActive(bool active);

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* MenuButton;

	UFUNCTION()
	void OnMenuButtonPressed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnDelay = 0.5f;

	UFUNCTION()
	void SetupOnPlayerDeath();

private:
	FTimerHandle RespawnDelayHandle;
	void OnRespawnDelayComplete();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/TextBlock.h"
#include "LevelUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API ULevelUIWidget : public UBaseUIScreen
{
	GENERATED_BODY()
	
public:
	virtual void Initialise() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UFUNCTION()
	void OnScoreUpdate();

	UFUNCTION()
	void OnHealthUpdate();
};

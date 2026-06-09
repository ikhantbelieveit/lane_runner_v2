// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUIWidget.h"
#include "GI_LevelSystem.h"
#include "PlayerCharacter.h"
#include "GI_UIStateSystem.h"
#include "Kismet/GameplayStatics.h"

void ULevelUIWidget::Initialise()
{
	GI = Cast<UMyGameInstance>(GetGameInstance());
	GI->OnPlayerSet.AddDynamic(this, &ULevelUIWidget::OnPlayerSet);
	if (GI->PlayerRef)
	{
		OnPlayerSet(GI->PlayerRef);
	}

	auto* levelSystem = GI->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->OnScoreSet.AddDynamic(this, &ULevelUIWidget::OnScoreUpdate);
		levelSystem->OnPause.AddDynamic(this, &ULevelUIWidget::ShowPauseUI);
		levelSystem->OnUnpause.AddDynamic(this, &ULevelUIWidget::HidePauseUI);
	}

	if (PauseUIOverlay)
	{
		PauseUIOverlay->Initialise();
		PauseUIOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULevelUIWidget::OnPlayerSet(APlayerCharacter* player)
{
	player->OnHealthSet.AddDynamic(this, &ULevelUIWidget::OnHealthUpdate);
	player->OnDistanceSet.AddDynamic(this, &ULevelUIWidget::OnDistanceUpdate);
}

void ULevelUIWidget::OnScoreUpdate()
{
	int score = 0;
	auto* levelSystem = GI->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		score = levelSystem->GetScore();
	}


	if (ScoreText)
	{
		FString scoreString = FString::FromInt(score);
		FText text = FText::FromString(scoreString);
		ScoreText->SetText(text);
	}
}

void ULevelUIWidget::OnHealthUpdate(float health)
{
	if (HealthText)
	{
		FString healthString = FString::FromInt(health);
		FText text = FText::FromString(healthString);
		HealthText->SetText(text);
	}
}

void ULevelUIWidget::OnDistanceUpdate(float distance)
{
	if (DistanceValueText)
	{
		float distanceMeters = GI->PlayerRef->GetDistanceTravelled_Meters();
		FString distanceString = FString::FromInt(distanceMeters);
		FText text = FText::FromString(distanceString);
		DistanceValueText->SetText(text);
	}
}

void ULevelUIWidget::ShowPauseUI()
{
	if (PauseUIOverlay)
	{
		PauseUIOverlay->SetVisibility(ESlateVisibility::Visible);
		
		if (auto* uiSystem = GI->GetSubsystem<UGI_UIStateSystem>())
		{
			uiSystem->ApplyInputMode(PauseUIOverlay->PauseInputMode);
		}

		DefaultSelection = PauseUIOverlay->ResumeButton;
		PauseUIOverlay->ResumeButton->SetKeyboardFocus();
	}
}

void ULevelUIWidget::HidePauseUI()
{
	if (PauseUIOverlay)
	{
		PauseUIOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	DefaultSelection = nullptr;
}

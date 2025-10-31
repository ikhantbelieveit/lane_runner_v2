// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathScreenUIWidget.h"
#include "GI_LevelSystem.h"
#include "GI_SaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GI_UIStateSystem.h"

void UDeathScreenUIWidget::Initialise()
{
	if (RespawnButton)
	{
		DefaultSelection = RespawnButton;
		RespawnButton->BroadcastButtonClick.AddDynamic(this, &UDeathScreenUIWidget::OnRespawnButtonPressed);
	}

	if (QuitButton)
	{
		QuitButton->BroadcastButtonClick.AddDynamic(this, &UDeathScreenUIWidget::OnQuitButtonPressed);
	}

	if (MenuButton)
	{
		MenuButton->BroadcastButtonClick.AddDynamic(this, &UDeathScreenUIWidget::OnMenuButtonPressed);
	}

	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
	{
		levelSystem->OnPlayerLose.AddDynamic(this, &UDeathScreenUIWidget::SetupOnPlayerDeath);
	}
}

void UDeathScreenUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UDeathScreenUIWidget::OnRespawnButtonPressed()
{
	GetWorld()->GetTimerManager().SetTimer(
		RespawnDelayHandle,
		this,
		&UDeathScreenUIWidget::OnRespawnDelayComplete,
		RespawnDelay,
		false,
		-1.0f
	);
}

void UDeathScreenUIWidget::OnMenuButtonPressed()
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->ExitLevel();
	}

	auto* uiStateSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::MainMenu);
	}
}

void UDeathScreenUIWidget::SetupOnPlayerDeath()
{
	FLevelScoreResult scoreResult;

	int basePoints = 0;
	int totalScore = 0;
	int distanceTravelled = 0;
	int distanceBonus = 0;
	int highScore = 0;
	bool newHighScore = false;

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		scoreResult = levelSystem->GetScoreResult();
		basePoints = scoreResult.BasePoints;
		totalScore = scoreResult.TotalScore;
		distanceTravelled = scoreResult.DistanceBonus.BonusValue;
		distanceBonus = scoreResult.DistanceBonus.TotalBonus;
		highScore = levelSystem->PrevSaveStatsCache.HighScore;
	}

	newHighScore = totalScore > highScore;

	if (newHighScore)
	{
		highScore = totalScore;
	}

	SetBasePointsText(basePoints);
	SetDistanceValueText(distanceTravelled);
	SetDistanceBonusText(distanceBonus);
	SetScoreTotalText(totalScore);
	SetHighScoreText(highScore);
	SetMessageActive(newHighScore);
}

void UDeathScreenUIWidget::OnRespawnDelayComplete()
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->ResetFromLose();
	}
}

void UDeathScreenUIWidget::OnQuitButtonPressed()
{
	auto* uiStateSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->QuitGame();
	}
}

void UDeathScreenUIWidget::OnScreenShown()
{
	Super::OnScreenShown();

	RespawnButton->SetKeyboardFocus();
}

void UDeathScreenUIWidget::OnScreenHidden()
{
	Super::OnScreenHidden();
}

void UDeathScreenUIWidget::SetBasePointsText(int basePoints)
{
	FText text = FText::FromString(FString::FromInt(basePoints));
	BaseScoreText->SetText(text);
}

void UDeathScreenUIWidget::SetDistanceValueText(int distance)
{
	FText text = FText::FromString(FString::FromInt(distance) + "M");
	DistanceValueText->SetText(text);
}

void UDeathScreenUIWidget::SetDistanceBonusText(int bonusVal)
{
	FText text = FText::FromString(FString::FromInt(bonusVal));
	DistanceBonusText->SetText(text);
}

void UDeathScreenUIWidget::SetScoreTotalText(int pointsTotal)
{
	FText text = FText::FromString(FString::FromInt(pointsTotal));
	TotalScoreText->SetText(text);
}

void UDeathScreenUIWidget::SetHighScoreText(int newHighScore)
{
	FText scoreText = FText::FromString(FString::FromInt(newHighScore));
	HighScoreText->SetText(scoreText);
}

void UDeathScreenUIWidget::SetMessageActive(bool active)
{
	if (active)
	{
		NewHighScoreMessageText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		NewHighScoreMessageText->SetVisibility(ESlateVisibility::Hidden);
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathScreenUIWidget.h"
#include "GI_LevelSystem.h"
#include "GI_SaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UDeathScreenUIWidget::Initialise()
{
	if (RespawnButton)
	{
		DefaultSelection = RespawnButton;
		RespawnButton->OnClicked.AddDynamic(this, &UDeathScreenUIWidget::OnRespawnButtonPressed);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UDeathScreenUIWidget::OnQuitButtonPressed);
	}
}

void UDeathScreenUIWidget::OnRespawnButtonPressed()
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->ResetFromLose();
	}
}

void UDeathScreenUIWidget::OnQuitButtonPressed()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}

void UDeathScreenUIWidget::SetupBeforeShow()
{
	Super::SetupBeforeShow();

	int currentScore = 0;
	int highScore = 0;
	bool newHighScore = false;

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		currentScore = levelSystem->GetScore();
	}

	auto* saveSystem = GetGameInstance()->GetSubsystem<UGI_SaveSystem>();
	if (saveSystem)
	{
		if (saveSystem->HasExistingSave())
		{
			highScore = saveSystem->CurrentSave->StatsData.HighScore;
		}
	}

	newHighScore = currentScore > highScore;

	if (newHighScore)
	{
		highScore = currentScore;
	}

	SetScoreText(currentScore);
	SetHighScoreText(highScore);
	SetMessageActive(newHighScore);
}

void UDeathScreenUIWidget::SetScoreText(int newScore)
{
	FText text = FText::FromString(FString::FromInt(newScore));
	ScoreText->SetText(text);
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

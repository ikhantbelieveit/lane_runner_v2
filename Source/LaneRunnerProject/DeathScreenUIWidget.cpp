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
		levelSystem->OnPlayerLose.AddDynamic(this, &UDeathScreenUIWidget::SetupBeforeShow);
	}
}

void UDeathScreenUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*ESlateVisibility respawnArrowVis = RespawnButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ESlateVisibility quitArrowVis = QuitButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ESlateVisibility menuArrowVis = MenuButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	RespawnButtonArrow->SetVisibility(respawnArrowVis);
	QuitButtonArrow->SetVisibility(quitArrowVis);
	MenuButtonArrow->SetVisibility(menuArrowVis);*/
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
	auto* uiStateSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::MainMenu);
	}
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
		highScore = levelSystem->HighScoreAtTimeOfDeath;
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

void UDeathScreenUIWidget::OnScreenShown()
{
	Super::OnScreenShown();

	RespawnButton->SetKeyboardFocus();
}

void UDeathScreenUIWidget::OnScreenHidden()
{
	Super::OnScreenHidden();
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuUIWidget.h"
#include "GI_UIStateSystem.h"
#include "GI_LevelSystem.h"

void UMainMenuUIWidget::Initialise()
{
	if (StartButton)
	{
		DefaultSelection = StartButton;
		StartButton->OnClicked.AddDynamic(this, &UMainMenuUIWidget::OnStartButtonPressed);
		StartButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuUIWidget::OnPlayButtonPressed);
		PlayButton->SetVisibility(ESlateVisibility::Hidden);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuUIWidget::OnQuitButtonPressed);
		QuitButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ESlateVisibility startArrowShow = StartButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ESlateVisibility playArrowShow = PlayButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ESlateVisibility quitArrowShow = QuitButton->HasKeyboardFocus() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	StartButtonArrow->SetVisibility(startArrowShow);
	PlayButtonArrow->SetVisibility(playArrowShow);
	QuitButtonArrow->SetVisibility(quitArrowShow);
}

void UMainMenuUIWidget::OnStartButtonPressed()
{
	StartButton->SetVisibility(ESlateVisibility::Hidden);
	PlayButton->SetVisibility(ESlateVisibility::Visible);
	QuitButton->SetVisibility(ESlateVisibility::Visible);

	DefaultSelection = PlayButton;
	PlayButton->SetKeyboardFocus();
}

void UMainMenuUIWidget::OnPlayButtonPressed()
{
	/*GetWorld()->GetTimerManager().SetTimer(
		StartGameDelayHandle,
		this,
		&UMainMenuUIWidget::OnStartGameDelayComplete,
		StartGameDelay,
		false,
		-1.0f
	);*/
	auto* uiStateSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::CharacterSelect);
	}
}

void UMainMenuUIWidget::OnQuitButtonPressed()
{
	auto* uiStateSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->QuitGame();
	}
}

//void UMainMenuUIWidget::OnStartGameDelayComplete()
//{
//	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
//	if (uiStateSystem)
//	{
//		uiStateSystem->EnterScreen(EUIState::TestLevel);
//	}
//
//	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
//	if (levelSystem)
//	{
//		levelSystem->EnterLevel();
//	}
//}

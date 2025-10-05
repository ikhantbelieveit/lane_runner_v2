// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuUIWidget.h"
#include "GI_UIStateSystem.h"
#include "GI_LevelSystem.h"

void UMainMenuUIWidget::Initialise()
{
	if (StartButton)
	{
		DefaultSelection = StartButton;
		StartButton->BroadcastButtonClick.AddDynamic(this, &UMainMenuUIWidget::OnStartButtonPressed);
		StartButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (PlayButton)
	{
		PlayButton->BroadcastButtonClick.AddDynamic(this, &UMainMenuUIWidget::OnPlayButtonPressed);
		PlayButton->SetVisibility(ESlateVisibility::Hidden);
	}

	if (QuitButton)
	{
		QuitButton->BroadcastButtonClick.AddDynamic(this, &UMainMenuUIWidget::OnQuitButtonPressed);
		QuitButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void UMainMenuUIWidget::OnScreenShown()
{
	if (StartButton->IsVisible())
	{
		StartButton->SetKeyboardFocus();
	}
	else
	{
		PlayButton->SetKeyboardFocus();
	}
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

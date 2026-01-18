// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUIWidget.h"
#include "GI_LevelSystem.h"
#include "GI_UIStateSystem.h"

void UPauseUIWidget::Initialise()
{
	if (ResumeButton)
	{
		ResumeButton->BroadcastButtonClick.AddDynamic(this, &UPauseUIWidget::OnResumeButtonPressed);
	}

	if (RestartButton)
	{
		RestartButton->BroadcastButtonClick.AddDynamic(this, &UPauseUIWidget::OnRestartButtonPressed);
	}

	if (SettingsButton)
	{
		SettingsButton->BroadcastButtonClick.AddDynamic(this, &UPauseUIWidget::OnSettingsButtonPressed);
	}

	if (QuitToMenuButton)
	{
		QuitToMenuButton->BroadcastButtonClick.AddDynamic(this, &UPauseUIWidget::OnQuitToMenuButtonPressed);
	}

	if (QuitToDesktopButton)
	{
		QuitToDesktopButton->BroadcastButtonClick.AddDynamic(this, &UPauseUIWidget::OnQuitToDesktopButtonPressed);
	}
}

void UPauseUIWidget::OnResumeButtonPressed(UUIButtonWidget* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("resume"));
	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
	{
		levelSystem->TogglePause();
	}
}

void UPauseUIWidget::OnRestartButtonPressed(UUIButtonWidget* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("restart"));
	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
	{
		levelSystem->RestartLevel();
	}
}

void UPauseUIWidget::OnSettingsButtonPressed(UUIButtonWidget* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("settings"));
}

void UPauseUIWidget::OnQuitToMenuButtonPressed(UUIButtonWidget* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("quit to menu"));
	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
	{
		levelSystem->TogglePause();
		levelSystem->ExitLevel();
	}

	if (auto* uiSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>())
	{
		uiSystem->EnterScreen(EUIState::MainMenu);
	}
}

void UPauseUIWidget::OnQuitToDesktopButtonPressed(UUIButtonWidget* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("quit to desktop"));
	if (auto* uiSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_UIStateSystem>())
	{
		uiSystem->QuitGame();
	}
}

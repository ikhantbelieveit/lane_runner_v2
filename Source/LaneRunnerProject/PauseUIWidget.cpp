// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUIWidget.h"
#include "GI_LevelSystem.h"

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

void UPauseUIWidget::OnResumeButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("resume"));
	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
	{
		levelSystem->TogglePause();
	}
}

void UPauseUIWidget::OnRestartButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("restart"));
}

void UPauseUIWidget::OnSettingsButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("settings"));
}

void UPauseUIWidget::OnQuitToMenuButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("quit to menu"));
}

void UPauseUIWidget::OnQuitToDesktopButtonPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("quit to desktop"));
}

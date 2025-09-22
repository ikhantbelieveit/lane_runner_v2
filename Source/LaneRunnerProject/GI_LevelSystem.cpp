// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelSystem.h"
#include "GI_UIStateSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GI_AudioSystem.h"
#include "GI_SaveSystem.h"

void UGI_LevelSystem::SetGameState(EGameState newState)
{
	if (CurrentGameState == newState)
	{
		return;
	}

	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();

	switch (newState)
	{
	case EGameState::Lose:

		if (uiStateSystem)
		{
			uiStateSystem->EnterScreen(EUIState::DeathScreen);
		}

		SaveLevelStats();

		break;
	case EGameState::Active:

		if (uiStateSystem)
		{
			uiStateSystem->EnterScreen(EUIState::TestLevel);
		}
		break;
	}

	PrevGameState = CurrentGameState;
	CurrentGameState = newState;

	OnGameStateChanged.Broadcast(CurrentGameState, PrevGameState);
}

EGameState UGI_LevelSystem::GetGameState()
{
	return CurrentGameState;
}

void UGI_LevelSystem::OnPlayerTouchHazard(bool oneHitKill, bool overrideInvincibility)
{
	if (GetGameState() == EGameState::Active)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
		if (player)
		{
			if (player->GetMercyInvincibleActive() && !overrideInvincibility)
			{
				return;
			}

			player->OnHitHazard(oneHitKill);
			if (player->GetCurrentHealth() <= 0)
			{
				SetGameState(EGameState::Lose);
			}

			auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
			if (audioSystem)
			{
				audioSystem->Play(EAudioKey::TakeDamage);
			}
		}
	}
}

void UGI_LevelSystem::ResetFromLose()
{
	CleanupBeforeReset.Broadcast();
	SetScore(0);

	SetGameState(EGameState::Active);
}

void UGI_LevelSystem::KillPlayer()
{
	SetGameState(EGameState::Lose);
}

void UGI_LevelSystem::OnPitfall()
{
	KillPlayer();
}

void UGI_LevelSystem::SetScore(int newScore)
{
	CurrentScore = newScore;

	OnScoreSet.Broadcast();
}

void UGI_LevelSystem::AddToScore(int addValue)
{
	SetScore(CurrentScore + addValue);
}

int UGI_LevelSystem::GetScore()
{
	return CurrentScore;
}

void UGI_LevelSystem::EnterLevel()
{
	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::TestLevel);
	}

	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->ResetPlayer();
	}

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	if (audioSystem)
	{
		audioSystem->Play(EAudioKey::BackgroundMusic);
	}

	SetScore(0);

	SetGameState(EGameState::Active);
}

void UGI_LevelSystem::TriggerContinue()
{
	if (GetGameState() == EGameState::AwaitContinue)
	{
		SetGameState(EGameState::Active);
	}
}

void UGI_LevelSystem::SaveLevelStats()
{
	int highScore = 0;

	auto* saveSystem = GetGameInstance()->GetSubsystem<UGI_SaveSystem>();
	if (saveSystem)
	{
		if (saveSystem->HasExistingSave())
		{
			highScore = saveSystem->CurrentSave->StatsData.HighScore;
		}
	}

	if (GetScore() > highScore)
	{
		highScore = GetScore();
	}
	if (saveSystem)
	{
		UMySaveGame* saveObject = Cast<UMySaveGame>(
			UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

		// Fill in some data
		saveObject->StatsData.HighScore = highScore;
		saveSystem->SaveGame(saveObject);

	};
}

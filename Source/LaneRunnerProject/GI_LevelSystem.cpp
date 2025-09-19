// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelSystem.h"
#include "UIStateSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveSystem.h"

void UGI_LevelSystem::SetGameState(EGameState newState)
{
	if (CurrentGameState == newState)
	{
		return;
	}

	AUIStateSystem* foundSystem = Cast<AUIStateSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIStateSystem::StaticClass()));

	switch (newState)
	{
	case EGameState::Lose:
		if (foundSystem)
		{
			foundSystem->EnterScreen(EUIState::DeathScreen);
		}

		SaveLevelStats();

		break;
	case EGameState::Active:

		if (foundSystem)
		{
			foundSystem->EnterScreen(EUIState::TestLevel);
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
	AUIStateSystem* uiSystem = Cast<AUIStateSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIStateSystem::StaticClass()));
	if (uiSystem)
	{
		//show test ui
		uiSystem->EnterScreen(EUIState::TestLevel);
	}

	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->ResetPlayer();

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

	AGameSaveSystem* foundGameSaveSystem = Cast<AGameSaveSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameSaveSystem::StaticClass()));
	if (foundGameSaveSystem)
	{
		if (foundGameSaveSystem->HasExistingSave())
		{
			highScore = foundGameSaveSystem->CurrentSave->StatsData.HighScore;
		}
	}

	if (GetScore() > highScore)
	{
		highScore = GetScore();
	}

	AGameSaveSystem* saveSystem = Cast<AGameSaveSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameSaveSystem::StaticClass()));
	if (saveSystem)
	{
		UMySaveGame* saveObject = Cast<UMySaveGame>(
			UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

		// Fill in some data
		saveObject->StatsData.HighScore = highScore;
		saveSystem->SaveGame(saveObject);

	};
}

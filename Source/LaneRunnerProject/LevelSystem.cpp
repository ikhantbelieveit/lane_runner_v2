// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSystem.h"
#include "UIStateSystem.h"
#include "PlayerCharacter.h"
#include "GameInit.h"
#include "Kismet/GameplayStatics.h"

void ALevelSystem::BeginPlay()
{
	Super::BeginPlay();

}

void ALevelSystem::SetGameState(EGameState newState)
{
	if(CurrentGameState == newState)
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

EGameState ALevelSystem::GetGameState()
{
	return CurrentGameState;
}

void ALevelSystem::OnPlayerTouchHazard()
{
	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->OnHitHazard();
		if (player->GetCurrentHealth() <= 0)
		{
			SetGameState(EGameState::Lose);
		}
		else
		{
			SetGameState(EGameState::AwaitContinue);
		}
	}
}

void ALevelSystem::ResetFromLose()
{
	CleanupBeforeReset.Broadcast();
	SetScore(0);

	SetGameState(EGameState::Active);
}

void ALevelSystem::KillPlayer()
{
	SetGameState(EGameState::Lose);
}

void ALevelSystem::OnPitfall()
{
	KillPlayer();
}

void ALevelSystem::SetScore(int newScore)
{
	CurrentScore = newScore;

	OnScoreSet.Broadcast();
}

void ALevelSystem::AddToScore(int addValue)
{
	SetScore(CurrentScore + addValue);
}

int ALevelSystem::GetScore()
{
	return CurrentScore;
}

void ALevelSystem::EnterLevel()
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

void ALevelSystem::TriggerContinue()
{
	if (GetGameState() == EGameState::AwaitContinue)
	{
		SetGameState(EGameState::Active);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSystem.h"
#include "PlayerCharacter.h"
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

	CurrentGameState = newState;
}

EGameState ALevelSystem::GetGameState()
{
	return CurrentGameState;
}

void ALevelSystem::OnPlayerTouchHazard()
{
	//TODO: deplete health instead of instant lose
	SetGameState(EGameState::Lose);
}

void ALevelSystem::ResetFromLose()
{
	//clear all projectiles
	//bring back destroyed objects & collected items
	//reset score
	//move player back to start
	//set player health
	CleanupBeforeReset.Broadcast();
	SetScore(0);

	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->OnLevelResetFromLose();
	}
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
	FString message = "set score to " + FString::FromInt(CurrentScore);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, message);
}

void ALevelSystem::AddToScore(int addValue)
{
	SetScore(CurrentScore + addValue);
}

int ALevelSystem::GetScore()
{
	return CurrentScore;
}

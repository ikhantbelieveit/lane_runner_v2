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

	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->OnLevelResetFromLose();
	}
	SetGameState(EGameState::Active);
}

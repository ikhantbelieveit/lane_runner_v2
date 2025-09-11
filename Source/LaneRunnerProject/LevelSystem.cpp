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

	//APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	//if (player)
	//{
	//	//foundLevelSystem->OnPlayerTouchHazard();
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PLAYER HIT - DO STUFF IN LEVEL SYSTEM"));
	//}
}

void ALevelSystem::ResetFromLose()
{
	//clear all projectiles
	//bring back destroyed objects & collected items
	//reset score
	//move player back to start
	//set player health
	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->OnLevelResetFromLose();
	}
	SetGameState(EGameState::Active);
}

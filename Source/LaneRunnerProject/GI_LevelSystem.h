// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GI_LevelSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelSystemEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameStateEvent, EGameState, newState, EGameState, prevState);

UCLASS()
class LANERUNNERPROJECT_API UGI_LevelSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	EGameState CurrentGameState = EGameState::Dormant;
	EGameState PrevGameState = EGameState::Dormant;

	int CurrentScore;

	FTimerHandle GameOverDelayHandle;
	void OnGameOverDelayComplete();

	float GameOverDelay = 0.75f;

	int PointsHealThreshold = 100;	//how many points the player needs to get a heal

public:
	void SetGameState(EGameState newState);
	EGameState GetGameState();

	void OnPlayerTouchHazard(bool oneHitKill, bool overrideInvincibility);

	void ResetFromLose();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FLevelSystemEvent CleanupBeforeReset;

	void KillPlayer();
	void OnPitfall();

	void SetScore(int newScore);
	void AddToScore(int addValue);


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FLevelSystemEvent OnScoreSet;

	UFUNCTION(BlueprintCallable)
	int GetScore();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameStateEvent OnGameStateChanged;

	UFUNCTION(BlueprintCallable)
	void EnterLevel();

	UFUNCTION(BlueprintCallable)
	void TriggerContinue();

	void SaveLevelStats();
};

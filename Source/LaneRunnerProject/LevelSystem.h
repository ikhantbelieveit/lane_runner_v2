// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameState.h"
#include "BaseGameSystem.h"
#include "LevelSystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelSystemEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameStateEvent, EGameState, newState, EGameState, prevState);

UCLASS()
class LANERUNNERPROJECT_API ALevelSystem : public ABaseGameSystem
{
	GENERATED_BODY()
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	EGameState CurrentGameState;
	EGameState PrevGameState;

	int CurrentScore;

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

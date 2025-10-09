// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELevelEventType.h"
#include "EProjectileDirection.h"
#include "GI_LevelSystem.generated.h"


USTRUCT(BlueprintType)
struct FLevelEventData
{
	GENERATED_BODY()

	/** What type of event this is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	ELevelEventType EventType;

	/** Target actor(s) to affect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	TArray<TObjectPtr<AActor>> TargetActors;

	//Target actor IDs - for chunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	TArray<FName> TargetActorIDs;

	/** Optional float/int parameter (e.g., distance along path, toggle state) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	float NumericParam = 0.f;

	/** Optional name/tag parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	FName TagParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	bool BoolParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	TObjectPtr<AActor> ActorParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	FVector VectorParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	EProjectileDirection DirectionParam = EProjectileDirection::None;
};

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

	int PointsHealThreshold = 500;	//how many points the player needs to get a heal

	int PointsUntilNextThreshold = PointsHealThreshold;

	void ExecuteSingleEvent(const FLevelEventData& Event);

	

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

	UFUNCTION(BlueprintCallable)
	void ResetLevel();

	UFUNCTION(BlueprintCallable)
	void ExecuteEvents(const TArray<FLevelEventData>& Events);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FLevelSystemEvent OnPlayerLose;

	int HighScoreAtTimeOfDeath;

	UFUNCTION(BlueprintCallable)
	void TogglePause();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FLevelSystemEvent OnPause;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FLevelSystemEvent OnUnpause;
};

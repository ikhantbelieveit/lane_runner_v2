// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameState.h"
#include "BaseGameSystem.h"
#include "LevelSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API ALevelSystem : public ABaseGameSystem
{
	GENERATED_BODY()
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	EGameState CurrentGameState;

public:
	void SetGameState(EGameState newState);
	EGameState GetGameState();

	void OnPlayerTouchHazard();
};

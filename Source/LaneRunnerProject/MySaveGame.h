// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FOptionsData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FProgressData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStatsData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int32 HighScore;
};

UCLASS()
class LANERUNNERPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FOptionsData OptionsData;

	UPROPERTY(BlueprintReadWrite)
	FProgressData ProgressData;

	UPROPERTY(BlueprintReadWrite)
	FStatsData StatsData;
	
	UPROPERTY(BlueprintReadWrite)
	int32 SaveVersion = 1;
};

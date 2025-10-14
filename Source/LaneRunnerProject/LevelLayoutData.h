// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelLayoutData.generated.h"


USTRUCT(BlueprintType)
struct FLevelChunkData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ChunkID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Index = -1;
};

USTRUCT(BlueprintType)
struct FLevelLayoutData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLevelChunkData> Chunks;
};

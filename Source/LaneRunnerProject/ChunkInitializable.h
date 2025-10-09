// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChunkSpawnEntry.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "ChunkInitializable.generated.h"

struct FChunkSpawnEntry;

UINTERFACE(BlueprintType)
class LANERUNNERPROJECT_API UChunkInitializable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be initialized from chunk data
 */
class LANERUNNERPROJECT_API IChunkInitializable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Chunk")
	void InitializeFromChunkData(const FChunkSpawnEntry& Entry);
};
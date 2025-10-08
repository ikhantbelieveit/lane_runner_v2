// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelChunkActor.h"
#include "LevelChunkDefinitionAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLevelChunkDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ChunkID;

	UPROPERTY(EditAnywhere)
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ALevelChunkActor> ChunkActor;
};

UCLASS()
class LANERUNNERPROJECT_API ULevelChunkDefinitionAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FLevelChunkDefinition Definition;
};

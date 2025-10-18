// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelChunkActor.h"
#include "ELevelChunkType.h"
#include "LevelChunkDefinitionAsset.generated.h"

USTRUCT(BlueprintType)
struct FChunkVariationSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SetID; // e.g. "Geometry", "Enemies"

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PossibleVariants;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandom = true;
};

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

	UPROPERTY(EditAnywhere)
	ELevelChunkType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChunkVariationSet> VariationSets;

public:
	FORCEINLINE float GetWeight() const { return Weight; }
};

UCLASS()
class LANERUNNERPROJECT_API ULevelChunkDefinitionAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FLevelChunkDefinition Definition;
};

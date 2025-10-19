// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelChunkActor.h"
#include "ELevelChunkType.h"
#include "LevelGenerationSettings.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FLevelGenerationSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	int GeneralChunkCount;

	UPROPERTY(EditAnywhere)
	bool UseOverrideSeed = false;

	UPROPERTY(EditAnywhere)
	int OverrideSeed;

	UPROPERTY(EditAnywhere)
	bool NoRepeatChunks = true;	//chunks are prevented from appearing twice in a row
};

UCLASS()
class LANERUNNERPROJECT_API ULevelGenerationSettingsAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FLevelGenerationSettings Definition;

	UFUNCTION(BlueprintCallable)
	int GetSeed() const {
		if (Definition.UseOverrideSeed)
		{
			return Definition.OverrideSeed;
		}

		FRandomStream random;
		return FMath::Rand();
	}
};

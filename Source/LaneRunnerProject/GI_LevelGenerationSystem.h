// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGenerationSettings.h"
#include "LevelLayoutData.h"
#include "LevelChunkDefinitionAsset.h"
#include "GI_LevelGenerationSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_LevelGenerationSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool GenerateLevelLayout(FLevelGenerationSettings settings, FRandomStream& random, FLevelLayoutData& outLevel);

	UFUNCTION(BlueprintCallable)
	void ResolveChunkVariants(const FLevelChunkDefinition& Definition, FRandomStream& Random, FLevelChunkData& OutChunk);

	UFUNCTION(BlueprintCallable)
	void ResolveMissingChunkVariants(FRandomStream& Random, FLevelLayoutData& InOutLayout);
};

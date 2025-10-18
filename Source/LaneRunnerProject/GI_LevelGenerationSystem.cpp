// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelGenerationSystem.h"
#include "GI_ChunkDefinitionLoadSystem.h"

bool UGI_LevelGenerationSystem::GenerateLevelLayout(FLevelGenerationSettings settings, FRandomStream& random, FLevelLayoutData& outLevel)
{
	outLevel.Chunks.Empty();

	int totalChunkIndex = 0;

	auto* chunkLoadSystem = GetGameInstance()->GetSubsystem<UGI_ChunkDefinitionLoadSystem>();

	TArray<FName> excludeChunks;
	
	FLevelChunkDefinition startDef;
	if (!chunkLoadSystem->GetRandomDefOfType(ELevelChunkType::Start, random, excludeChunks, startDef))
	{
		//complain here
		return false;
	}

	FLevelChunkData startChunk;
	startChunk.ChunkID = startDef.ChunkID;
	startChunk.Index = totalChunkIndex;
	totalChunkIndex++;
	outLevel.Chunks.Add(startChunk);

	for (int chunkIndex = 0; chunkIndex <= settings.GeneralChunkCount; ++chunkIndex)
	{
		FLevelChunkDefinition generalChunkDef;
		if (!chunkLoadSystem->GetRandomDefOfType(ELevelChunkType::General, random, excludeChunks, generalChunkDef))
		{
			//complain here

			return false;
		}

		FLevelChunkData generalChunk;
		generalChunk.ChunkID = generalChunkDef.ChunkID;
		generalChunk.Index = totalChunkIndex;
		totalChunkIndex++;

		ResolveChunkVariants(generalChunkDef, random, generalChunk);

		outLevel.Chunks.Add(generalChunk);

		excludeChunks.Empty();
		excludeChunks.Add(generalChunkDef.ChunkID);	//prevent a chunk from appearing twice
	}

	return true;
}

void UGI_LevelGenerationSystem::ResolveChunkVariants(const FLevelChunkDefinition& Definition, FRandomStream& Random, FLevelChunkData& OutChunk)
{
	for (const FChunkVariationSet& Set : Definition.VariationSets)
	{
		if (Set.PossibleVariants.Num() == 0)
			continue;

		FName ChosenVariant = NAME_None;

		if (Set.bUseRandom)
		{
			int32 Index = Random.RandRange(0, Set.PossibleVariants.Num() - 1);
			ChosenVariant = Set.PossibleVariants[Index];
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Magenta,
				FString::Printf(TEXT("chosen variant: %s"), *ChosenVariant.ToString())
			);
		}
		else
		{
			ChosenVariant = Set.PossibleVariants[0]; // deterministic fallback
		}

		if (!ChosenVariant.IsNone())
		{
			FChunkVariantEntry fallbackVar;

			fallbackVar.SetID = Set.SetID;
			fallbackVar.Variant = ChosenVariant;

			OutChunk.ActiveVariants.Add(fallbackVar);
		}
	}
}

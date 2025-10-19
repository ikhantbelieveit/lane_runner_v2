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

		if (settings.NoRepeatChunks)
		{
			excludeChunks.Add(generalChunkDef.ChunkID);	//prevent a chunk from appearing twice
		}
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

void UGI_LevelGenerationSystem::ResolveMissingChunkVariants(FRandomStream& Random, FLevelLayoutData& InOutLayout)
{
	auto* chunkDefSystem = GetGameInstance()->GetSubsystem<UGI_ChunkDefinitionLoadSystem>();

	for (FLevelChunkData& Chunk : InOutLayout.Chunks)
	{


		// Find the chunk definition this layout entry refers to
		FLevelChunkDefinition ChunkDef;
		if (!chunkDefSystem->GetChunkDefinition(Chunk.ChunkID, ChunkDef)) continue;

		// Reuse existing method for variant resolution
		for (const FChunkVariationSet& Set : ChunkDef.VariationSets)
		{
			// Skip if already defined in layout
			bool bAlreadyHasSet = false;
			for (const FChunkVariantEntry& Entry : Chunk.ActiveVariants)
			{
				if (Entry.SetID == Set.SetID)
				{
					bAlreadyHasSet = true;
					break;
				}
			}
			if (bAlreadyHasSet)
				continue;

			// Otherwise, pick one randomly
			FChunkVariantEntry NewEntry;
			NewEntry.SetID = Set.SetID;

			if (Set.PossibleVariants.Num() > 0)
			{
				if (Set.bUseRandom)
				{
					int32 Index = Random.RandRange(0, Set.PossibleVariants.Num() - 1);
					NewEntry.Variant = Set.PossibleVariants[Index];
				}
				else
				{
					NewEntry.Variant = Set.PossibleVariants[0];
				}

				Chunk.ActiveVariants.Add(NewEntry);
			}
		}
	}
}

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

		excludeChunks.Empty();
		excludeChunks.Add(generalChunkDef.ChunkID);	//prevent a chunk from appearing twice

		outLevel.Chunks.Add(generalChunk);
	}

	return true;
}

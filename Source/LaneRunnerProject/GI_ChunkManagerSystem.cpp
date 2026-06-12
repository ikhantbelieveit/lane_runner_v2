// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_ChunkManagerSystem.h"
#include "LevelChunkDefinitionAsset.h"
#include "Kismet/GameplayStatics.h"
#include "GI_ChunkDefinitionLoadSystem.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "GI_LevelSystem.h"
#include "GI_LevelChunkPoolSystem.h"

void UGI_ChunkManagerSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UGI_LevelSystem>();

	GI = Cast<UMyGameInstance>(GetGameInstance());
	GI->OnPlayerSet.AddDynamic(this, &UGI_ChunkManagerSystem::OnPlayerSet);
	if (GI->PlayerRef)
	{
		OnPlayerSet(GI->PlayerRef);
	}

	auto* levelSystem = GI->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->OnLevelExit.AddDynamic(this, &UGI_ChunkManagerSystem::ClearChunks);
	}
}

void UGI_ChunkManagerSystem::OnPlayerSet(APlayerCharacter* player)
{
	player->OnDistanceSet.AddDynamic(this, &UGI_ChunkManagerSystem::OnPlayerDistanceUpdate);
}

void UGI_ChunkManagerSystem::Deinitialize()
{
	GI->WorldRef->GetTimerManager().ClearTimer(TickHandle);
	Super::Deinitialize();
}

void UGI_ChunkManagerSystem::InitFromLayoutData(FLevelLayoutData layoutData)
{
	if (layoutData.Chunks.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Layout Data has 0 chunks!"));
		return;
	}

	CurrentLayoutData = layoutData;
	InitLayoutDataSections();

	CurrentSection = GetSectionForCurrentIndex();
	SpawnChunksForCurrentSection();
}

void UGI_ChunkManagerSystem::InitLayoutDataSections()
{
	UGI_ChunkDefinitionLoadSystem* chunkLoadSystem = GI->GetSubsystem<UGI_ChunkDefinitionLoadSystem>();
	if (!chunkLoadSystem)
	{
		return;
	}

	LayoutDataSections.Empty();
	FLevelSection Section;

	for (const FLevelChunkData Chunk : CurrentLayoutData.Chunks)
	{
		Section.Chunks.Add(Chunk);
		FLevelChunkDefinition chunkDef;
		if (chunkLoadSystem->GetChunkDefinition(Chunk.ChunkID, chunkDef))
		{
			Section.SectionLength += chunkDef.Length;
		}
		if (Section.SectionLength >= MinSectionLength)
		{
			LayoutDataSections.Add(Section);
			Section = FLevelSection();
		}
	}

	//if final section remains with chunks < MinSectionLength, add to end of final section in LayoutDataSection
	if (!Section.Chunks.IsEmpty())
	{
		FLevelSection finalSection = LayoutDataSections.IsEmpty() ? FLevelSection() : LayoutDataSections.Last();
		for (const FLevelChunkData Chunk : Section.Chunks)
		{
			finalSection.Chunks.Add(Chunk);
			FLevelChunkDefinition chunkDef;
			if (chunkLoadSystem->GetChunkDefinition(Chunk.ChunkID, chunkDef))
			{
				finalSection.SectionLength += chunkDef.Length;
			}
		}
		if (!LayoutDataSections.IsEmpty())
		{
			int finalIndex = LayoutDataSections.Num() - 1;
			LayoutDataSections.RemoveAt(finalIndex);
		}
		LayoutDataSections.Add(finalSection);
	}

	CurrentSectionIndex = 0;
	SectionSpawnXPos = 0;
	NumLayoutDataSections = LayoutDataSections.Num();
}

FLevelSection UGI_ChunkManagerSystem::GetSectionForCurrentIndex()
{
	if (CurrentSectionIndex >= NumLayoutDataSections)
	{
		return LayoutDataSections[NumLayoutDataSections - 1];	//TODO: actually generate new sections on the fly instead of repeating last section endlessly
	}
	return LayoutDataSections[CurrentSectionIndex];
}

void UGI_ChunkManagerSystem::ClearChunks()
{
	GI->GetSubsystem<UGI_LevelChunkPoolSystem>()->ResetAllPools();

	ActiveChunkActors.Empty();
}

void UGI_ChunkManagerSystem::SpawnChunksForCurrentSection()
{
	FVector SpawnCursor = FVector(SectionSpawnXPos, 0, 0);
	UGI_ChunkDefinitionLoadSystem* chunkLoadSystem = GI->GetSubsystem<UGI_ChunkDefinitionLoadSystem>();
	if (!chunkLoadSystem)
	{
		return;
	}

	FLevelChunkDefinition chunkDef;

	UGI_LevelChunkPoolSystem* poolSystem = GI->GetSubsystem<UGI_LevelChunkPoolSystem>();

	for (FLevelChunkData Entry : CurrentSection.Chunks)
	{
		if (!chunkLoadSystem->GetChunkDefinition(Entry.ChunkID, chunkDef))
		{
			continue;
		}

		FTransform SpawnTransform = FTransform();
		SpawnTransform.AddToTranslation(SpawnCursor);
		Entry.SpawnLocation = SpawnTransform.GetLocation();

		ALevelChunkActor* NewChunk = poolSystem->RequestChunk(Entry);

		SpawnCursor += FVector(chunkDef.Length, 0.f, 0.f);
		ActiveChunkActors.Add(NewChunk);

		UE_LOG(LogTemp, Log, TEXT("[LEVEL] place chunk %s in level"), *Entry.ChunkID.ToString());
	}

	NextSectionIncrementThreshold = SectionSpawnXPos + CurrentSection.SectionLength - SectionEndDistanceThreshold;
	SectionSpawnXPos += CurrentSection.SectionLength;

}

void UGI_ChunkManagerSystem::OnPlayerDistanceUpdate(float distance)
{
	if (distance >= NextSectionIncrementThreshold)
	{
		IncrementSection();
	}
}

void UGI_ChunkManagerSystem::IncrementSection()
{
	CurrentSectionIndex++;
	CurrentSection = GetSectionForCurrentIndex();
	SpawnChunksForCurrentSection();
	UE_LOG(LogTemp, Log, TEXT("Increment to section %d"), CurrentSectionIndex);
}
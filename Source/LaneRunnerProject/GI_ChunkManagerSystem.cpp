// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_ChunkManagerSystem.h"
#include "LevelChunkDefinitionAsset.h"
#include "Kismet/GameplayStatics.h"
#include "GI_ChunkDefinitionLoadSystem.h"
#include "Components/BoxComponent.h"
#include "GI_LevelSystem.h"

void UGI_ChunkManagerSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UGI_LevelSystem>();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->OnLevelExit.AddDynamic(this, &UGI_ChunkManagerSystem::ClearChunks);
	}

	UE_LOG(LogTemp, Log, TEXT("Chunk Manager Subsystem initialized after LevelSystemSubsystem"));
}

void UGI_ChunkManagerSystem::SpawnChunksFromLayoutData(FLevelLayoutData layoutData)
{
	if (layoutData.Chunks.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No LevelLayoutData found!"));
		return;
	}

	const int32 RepeatCount = 1; // repeat this many times for now
	FVector SpawnCursor = FVector::ZeroVector;

	UWorld* World = GetWorld();
	if (!World) return;

	UGI_ChunkDefinitionLoadSystem* chunkLoadSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_ChunkDefinitionLoadSystem>();
	if (!chunkLoadSystem)
	{
		return;
	}

	FLevelChunkDefinition chunkDef;


	for (int32 LoopIndex = 0; LoopIndex < RepeatCount; ++LoopIndex)
	{
		for (const FLevelChunkData Entry : layoutData.Chunks)
		{
			if (!chunkLoadSystem->GetChunkDefinition(Entry.ChunkID, chunkDef))
			{
				continue;
			}

			FTransform SpawnTransform = FTransform();
			SpawnTransform.AddToTranslation(SpawnCursor);

			if (chunkDef.ChunkActor) // make sure it's valid
			{
				ALevelChunkActor* NewChunk = World->SpawnActorDeferred<ALevelChunkActor>(
					chunkDef.ChunkActor.Get(),       // UClass*
					SpawnTransform);

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("start spawning chunk %s"), *Entry.ChunkID.ToString()));

				if (NewChunk)
				{
					// Finish spawning
					UGameplayStatics::FinishSpawningActor(NewChunk, SpawnTransform);
					NewChunk->InitializeFromLayoutData(Entry);

					SpawnCursor += FVector(GetChunkLength(NewChunk), 0.f, 0.f);

					ActiveChunkActors.Add(NewChunk);
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("finish spawning chunk %s"), *Entry.ChunkID.ToString()));
				}

				
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("NO ACTOR FOUND for chunk %s"), *Entry.ChunkID.ToString()));
			}
		}
	}
}

void UGI_ChunkManagerSystem::ClearChunks()
{
	for (ALevelChunkActor* ChunkActor : ActiveChunkActors)
	{
		if (ChunkActor)
		{
			ChunkActor->Destroy();
		}
	}

	ActiveChunkActors.Empty();
}

float UGI_ChunkManagerSystem::GetChunkLength(ALevelChunkActor* chunkActor)
{
	if (!chunkActor) return 0.f;

	TArray<UBoxComponent*> BoxComponents;
	chunkActor->GetComponents(BoxComponents);

	for (UBoxComponent* BoxComp : BoxComponents)
	{
		if (BoxComp && BoxComp->ComponentHasTag(FName("BoundsBox")))
		{
			FVector BoxExtent = BoxComp->GetScaledBoxExtent();
			return BoxExtent.X * 2.f;
		}
	}

	return 0.f;
}

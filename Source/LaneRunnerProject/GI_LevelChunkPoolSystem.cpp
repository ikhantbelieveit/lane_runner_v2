// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelChunkPoolSystem.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GI_LevelSystem.h"
#include "GI_ChunkDefinitionLoadSystem.h"

void UGI_LevelChunkPoolSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GI = Cast<UMyGameInstance>(GetGameInstance());
}

void UGI_LevelChunkPoolSystem::InitOnChunkLoadComplete()
{
    TArray<FLevelChunkDefinition> allChunkDefs = GI->GetSubsystem<UGI_ChunkDefinitionLoadSystem>()->GetAllDefinitions();

    ClassLUT.Empty();
    Pools.Empty();

    for (const FLevelChunkDefinition Def : allChunkDefs)
    {
        ClassLUT.Add(Def.ChunkID, Def.ChunkActor);
        Pools.Add(Def.ChunkID, FChunkPool());

        for (int i = 0; i < InitialPoolSize; ++i)
        {
            ALevelChunkActor* chunk = CreateNewChunk(Def.ChunkID);
            if (chunk)
            {
                Pools[Def.ChunkID].Inactive.Add(chunk);
            }
        }
    }
}


ALevelChunkActor* UGI_LevelChunkPoolSystem::CreateNewChunk(FName ChunkID)
{
    if (!ClassLUT.Contains(ChunkID))
        return nullptr;

    TSubclassOf<ALevelChunkActor> ClassToSpawn = ClassLUT[ChunkID];

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ALevelChunkActor* NewChunk = GI->WorldRef->SpawnActorDeferred<ALevelChunkActor>(
        ClassToSpawn,
        FTransform(DormantChunkPos));

    if (NewChunk)
    {
        // Finish spawning
        UGameplayStatics::FinishSpawningActor(NewChunk, FTransform(DormantChunkPos));
        NewChunk->SetActorEnableCollision(false);
        NewChunk->SetActorHiddenInGame(true);
        NewChunk->SetActorTickEnabled(false);
    }

    return NewChunk;
}

ALevelChunkActor* UGI_LevelChunkPoolSystem::RequestChunk(const FLevelChunkData& Request)
{
    if (!ClassLUT.Contains(Request.ChunkID))
        return nullptr;

    FChunkPool& Pool = Pools.FindOrAdd(Request.ChunkID);

    // Find an inactive one to reuse
    for (int32 i = Pool.Inactive.Num() - 1; i >= 0; --i)
    {
        if (ALevelChunkActor* Chunk = Pool.Inactive[i].Get())
        {
            Pool.Inactive.Remove(Chunk);
            Pool.Active.Add(Chunk);

            Chunk->SetActorLocation(Request.SpawnLocation);
            Chunk->InitializeFromLayoutData(Request);
            Chunk->ResetChildren();
            Chunk->SetActorEnableCollision(true);
            Chunk->SetActorHiddenInGame(false);

            return Chunk;
        }
        else
        {
            Pool.Inactive.RemoveAt(i);
        }
    }

    // None available, create new
    ALevelChunkActor* NewChunk = CreateNewChunk(Request.ChunkID);
    if (NewChunk)
    {
        Pool.Active.Add(NewChunk);
        NewChunk->SetActorLocation(Request.SpawnLocation);
        NewChunk->InitializeFromLayoutData(Request);
        NewChunk->ResetChildren();
        NewChunk->SetActorEnableCollision(true);
        NewChunk->SetActorHiddenInGame(false);
    }
    return NewChunk;
}

void UGI_LevelChunkPoolSystem::ReturnChunk(ALevelChunkActor* Actor)
{
    if (!Actor) return;

    FName ChunkID = Actor->ChunkID;

    if (!Pools.Contains(ChunkID))
        return;

    FChunkPool& Pool = Pools[ChunkID];

    Pool.Active.Remove(Actor);
    Pool.Inactive.Add(Actor);

    Actor->TeardownChildren();

    Actor->SetActorLocation(DormantChunkPos);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorHiddenInGame(true);
    Actor->SetActorTickEnabled(false);
}

void UGI_LevelChunkPoolSystem::ResetAllPools()
{
    TMap<FName, FChunkPool> PoolsCopy = TMap<FName, FChunkPool>(Pools);

    for (auto& Elem : PoolsCopy)
    {
        FChunkPool& Pool = Elem.Value;

        for (auto& WeakCol : Pool.Active)
        {
            if (ALevelChunkActor* Chunk = WeakCol.Get())
            {
                ReturnChunk(Chunk);
            }
        }
    }
}

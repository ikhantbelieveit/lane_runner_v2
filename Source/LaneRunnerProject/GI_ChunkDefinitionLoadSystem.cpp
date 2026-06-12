// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_ChunkDefinitionLoadSystem.h"
#include "MyGameInstance.h"
#include "GI_LevelChunkPoolSystem.h"

void UGI_ChunkDefinitionLoadSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GI = Cast<UMyGameInstance>(GetGameInstance());

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TickHandle,
            [this]()
            {
                TickSubsystem(0.016f); // Approx 60 FPS
            },
            0.016f,
            true
        );
    }
}

void UGI_ChunkDefinitionLoadSystem::Deinitialize()
{
    GI->WorldRef->GetTimerManager().ClearTimer(TickHandle);
    Super::Deinitialize();
}

void UGI_ChunkDefinitionLoadSystem::TickSubsystem(float DeltaTime)
{
    if (!HasInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            HasInitialisedFromConfig = true;
        }
    }

}

bool UGI_ChunkDefinitionLoadSystem::InitialiseFromConfig()
{
    FChunkDefLoadConfig configData = GI->ConfigData->ChunkLoadConfig->ConfigData;
    ChunkDef_LUT.Empty();

    for (const ULevelChunkDefinitionAsset* Entry : configData.ChunkDefs)
    {
        ChunkDef_LUT.Add(Entry->Definition.ChunkID, Entry->Definition);
    }

    if (auto* poolSystem = GI->GetSubsystem<UGI_LevelChunkPoolSystem>())
    {
        poolSystem->InitOnChunkLoadComplete();
    }
    return true;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_CollectiblePoolSystem.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SpawnComponent.h"
#include "GI_LevelSystem.h"

void UGI_CollectiblePoolSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UGI_LevelSystem>();

    auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
    if (levelSystem)
    {
        levelSystem->CleanupBeforeReset.AddDynamic(this, &UGI_CollectiblePoolSystem::ResetAllPools);
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TickHandle,
            [this]()
            {
                TickSubsystem(0.016f);
            },
            0.016f,
            true
        );
    }
}

void UGI_CollectiblePoolSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }

    Super::Deinitialize();
}

void UGI_CollectiblePoolSystem::TickSubsystem(float DeltaTime)
{
    if (!bInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            bInitialisedFromConfig = true;
        }
    }
}

bool UGI_CollectiblePoolSystem::InitialiseFromConfig()
{
    UE_LOG(LogTemp, Warning, TEXT("!!! [POOL] InitialiseFromConfig CALLED !!!"));

    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (!GI || !GI->ConfigData || !GI->ConfigData->CollectiblePoolConfig)
        return false;

    UCollectiblePoolConfigData* Config = GI->ConfigData->CollectiblePoolConfig;

    ClassLUT.Empty();
    Pools.Empty();

    for (const FCollectiblePoolConfigEntry& Entry : Config->Entries)
    {
        ClassLUT.Add(Entry.Type, Entry.Class);
        Pools.Add(Entry.Type, FCollectiblePool());

        // Prewarm
        for (int32 i = 0; i < Entry.InitialPoolSize; ++i)
        {
            ABaseCollectible* Col = CreateNewCollectible(Entry.Type);
            if (Col)
            {
                Pools[Entry.Type].Inactive.Add(Col);
            }
        }
    }

    return true;
}

ABaseCollectible* UGI_CollectiblePoolSystem::CreateNewCollectible(ECollectibleType Type)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    if (!ClassLUT.Contains(Type))
        return nullptr;

    TSubclassOf<ABaseCollectible> ClassToSpawn = ClassLUT[Type];

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABaseCollectible* NewCol =
        World->SpawnActor<ABaseCollectible>(ClassToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (NewCol)
    {
        NewCol->bIsPooledInstance = true;

        if (USpawnComponent* SpawnComp = NewCol->FindComponentByClass<USpawnComponent>())
        {
            SpawnComp->Despawn();
        }
    }

    return NewCol;
}

ABaseCollectible* UGI_CollectiblePoolSystem::RequestCollectible(const FCollectibleRequest& Request)
{
    if (!ClassLUT.Contains(Request.Type))
        return nullptr;

    FCollectiblePool& Pool = Pools.FindOrAdd(Request.Type);

    // Find an inactive one to reuse
    for (int32 i = Pool.Inactive.Num() - 1; i >= 0; --i)
    {
        if (ABaseCollectible* Col = Pool.Inactive[i].Get())
        {
            Pool.Inactive.Remove(Col);
            Pool.Active.Add(Col);

            Col->SetActorLocation(Request.SpawnLocation);

            if (USpawnComponent* SpawnComp = Col->FindComponentByClass<USpawnComponent>())
            {
                SpawnComp->Spawn(true, Request.bShouldScroll, true);
            }

            return Col;
        }
        else
        {
            Pool.Inactive.RemoveAt(i);
        }
    }

    // None available, create new
    ABaseCollectible* NewCol = CreateNewCollectible(Request.Type);
    if (NewCol)
    {
        Pool.Active.Add(NewCol);
        NewCol->SetActorLocation(Request.SpawnLocation);

        if (USpawnComponent* SpawnComp = NewCol->FindComponentByClass<USpawnComponent>())
        {
            SpawnComp->Spawn(true, Request.bShouldScroll, true);
        }
    }

    return NewCol;
}

void UGI_CollectiblePoolSystem::ReturnCollectible(ABaseCollectible* Collectible)
{
    if (!Collectible || !Collectible->bIsPooledInstance)
    {
        // Not a pooled collectible
        return;
    }

    ECollectibleType Type = Collectible->Type;

    if (!Pools.Contains(Type))
        return;

    FCollectiblePool& Pool = Pools[Type];

    Pool.Active.Remove(Collectible);
    Pool.Inactive.Add(Collectible);
}

void UGI_CollectiblePoolSystem::ResetAllPools()
{
    for (auto& Elem : Pools)
    {
        FCollectiblePool& Pool = Elem.Value;

        for (auto& WeakCol : Pool.Active)
        {
            if (ABaseCollectible* Col = WeakCol.Get())
            {
                if (USpawnComponent* SpawnComp = Col->FindComponentByClass<USpawnComponent>())
                {
                    SpawnComp->Despawn();
                }

                Pool.Inactive.Add(Col);
            }
        }

        Pool.Active.Empty();
    }

    
}
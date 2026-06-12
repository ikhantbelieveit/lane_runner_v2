// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_CollectiblePoolSystem.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SpawnComponent.h"
#include "GI_LevelSystem.h"

void UGI_CollectiblePoolSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

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
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (!GI || !GI->ConfigData || !GI->ConfigData->CollectiblePoolConfig)
        return false;

    UCollectiblePoolConfigData* Config = GI->ConfigData->CollectiblePoolConfig;

    CommonSet = Config->CommonSet;

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
            UE_LOG(LogTemp, Log, TEXT("[HELLO] created new collectible %s"), *Col->GetName());
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

    ABaseCollectible* NewCol = GetWorld()->SpawnActorDeferred<ABaseCollectible>(
        ClassToSpawn,
        FTransform(FVector::ZeroVector));

    if (NewCol)
    {
        // Finish spawning
        UGameplayStatics::FinishSpawningActor(NewCol, FTransform(FVector::ZeroVector));
        NewCol->bIsPooledInstance = true;

        if (NewCol->Implements<UChunkInitializable>())
        {
            IChunkInitializable::Execute_InitializeFromChunk(NewCol);
        }

        if (USpawnComponent* SpawnComp = NewCol->FindComponentByClass<USpawnComponent>())
        {
            if (SpawnComp->Implements<UChunkInitializable>())
            {
                IChunkInitializable::Execute_InitializeFromChunk(SpawnComp);
            }
            SpawnComp->ResetAsSpawned = false;
            SpawnComp->Despawn();
        }

        if (ULocationManagerComponent* locManager = NewCol->FindComponentByClass<ULocationManagerComponent>())
        {
            if (locManager->Implements<UChunkInitializable>())
            {
                IChunkInitializable::Execute_InitializeFromChunk(locManager);
            }

            locManager->SetPhysicsDriven(true);
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
            if (Col->Implements<UChunkInitializable>())
            {
                IChunkInitializable::Execute_ResetOnChunkRequest(Col);
            }

            for (auto* comp : Col->GetComponents())
            {
                if (comp->Implements<UChunkInitializable>())
                {
                    IChunkInitializable::Execute_ResetOnChunkRequest(comp);
                }
            }

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
            SpawnComp->ResetAsSpawned = false;
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

    if (Collectible->Implements<UChunkInitializable>())
    {
        IChunkInitializable::Execute_TeardownFromChunk(Collectible);
    }

    for (auto* comp : Collectible->GetComponents())
    {
        if (comp->Implements<UChunkInitializable>())
        {
            IChunkInitializable::Execute_TeardownFromChunk(comp);
        }
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
    TMap<ECollectibleType, FCollectiblePool> PoolsCache = TMap<ECollectibleType, FCollectiblePool>(Pools);
    for (auto& Elem : PoolsCache)
    {
        FCollectiblePool& Pool = Elem.Value;

        TArray<TWeakObjectPtr<ABaseCollectible>> ActiveCache = TArray<TWeakObjectPtr<ABaseCollectible>>(Pool.Active);

        for (auto& WeakCol : ActiveCache)
        {
            if (ABaseCollectible* Col = WeakCol.Get())
            {
                ReturnCollectible(Col);
            }
        }

        Pool.Active.Empty();
    }
}

ABaseCollectible* UGI_CollectiblePoolSystem::RequestRandomCollectibleFromCommonSet(const FCollectibleRequest& Request)
{
    FRandomStream random(FMath::Rand());
    ECollectibleType type;
    if (!CommonSet->GetRandomCollectibleType(random, type))
    {
        //complain here
        return nullptr;
    }

    FCollectibleRequest randomTypeRequest;
    randomTypeRequest.bShouldScroll = Request.bShouldScroll;
    randomTypeRequest.SpawnLocation = Request.SpawnLocation;
    randomTypeRequest.Type = type;

    return RequestCollectible(randomTypeRequest);
}

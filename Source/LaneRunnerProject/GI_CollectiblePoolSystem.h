// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ECollectibleType.h"
#include "CollectibleRequest.h"
#include "BaseCollectible.h"
#include "CollectibleSet.h"
#include "GI_CollectiblePoolSystem.generated.h"

USTRUCT()
struct FCollectiblePool
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TWeakObjectPtr<ABaseCollectible>> Inactive;

    UPROPERTY()
    TArray<TWeakObjectPtr<ABaseCollectible>> Active;
};

UCLASS()
class UGI_CollectiblePoolSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void TickSubsystem(float DeltaTime);
    bool InitialiseFromConfig();

    ABaseCollectible* RequestCollectible(const FCollectibleRequest& Request);
    void ReturnCollectible(ABaseCollectible* Collectible);
    void ResetAllPools();

    ABaseCollectible* RequestRandomCollectibleFromCommonSet(const FCollectibleRequest& Request);

private:
    bool bInitialisedFromConfig = false;

    FTimerHandle TickHandle;

    // Maps collectible type to loaded class
    TMap<ECollectibleType, TSubclassOf<ABaseCollectible>> ClassLUT;

    // Maps collectible type to pooled instances
    TMap<ECollectibleType, FCollectiblePool> Pools;

    ABaseCollectible* CreateNewCollectible(ECollectibleType Type);

    TObjectPtr<UCollectibleSet> CommonSet;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelChunkActor.h"
#include "MyGameInstance.h"
#include "GI_LevelChunkPoolSystem.generated.h"

/**
 * 
 */
USTRUCT()
struct FChunkPool
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TWeakObjectPtr<ALevelChunkActor>> Inactive;

    UPROPERTY()
    TArray<TWeakObjectPtr<ALevelChunkActor>> Active;
};

UCLASS()
class LANERUNNERPROJECT_API UGI_LevelChunkPoolSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    ALevelChunkActor* RequestChunk(const FLevelChunkData& Request);
    void ReturnChunk(ALevelChunkActor* Actor);

    UFUNCTION()
    void ResetAllPools();

    UFUNCTION()
    void InitOnChunkLoadComplete();

private:
    bool bInitialisedFromConfig = false;

    FTimerHandle TickHandle;

    // Maps collectible type to loaded class
    TMap<FName, TSubclassOf<ALevelChunkActor>> ClassLUT;

    // Maps collectible type to pooled instances
    TMap<FName, FChunkPool> Pools;

    ALevelChunkActor* CreateNewChunk(FName ChunkID);
    UMyGameInstance* GI;

    const int InitialPoolSize = 1;
    const FVector DormantChunkPos = FVector(0, -9999, -9999);//FVector::Zero();//FVector(0, -9999, -9999);
};

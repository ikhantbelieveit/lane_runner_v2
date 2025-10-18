// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkSpawnEntry.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FChunkSpawnEntry
{
    GENERATED_BODY()

    // The type of actor to spawn (can be anything)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ActorClass;

    // Position/orientation within the chunk
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform RelativeTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSetScale;

    // Optional ID or name for reference (used by event binding, debugging, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ActorID;

    // Optional JSON-style metadata for specialization
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Metadata; // e.g. {"EnemyType":"Bullseye","Speed":300,"Scroll":true}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants")
    FName VariantSet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants")
    FName VariantOption;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECollectibleType.h"
#include "CollectibleRequest.generated.h"

USTRUCT(BlueprintType)
struct FCollectibleRequest
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECollectibleType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector SpawnLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShouldScroll = false;
};
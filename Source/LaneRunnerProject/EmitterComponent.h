// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmitterComponent.generated.h"

class UTimedActionComponent;

USTRUCT(BlueprintType)
struct FSpawnEntry
{
    GENERATED_BODY()

    // Actor class to spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<AActor> ActorClass = nullptr;

    // Offset from the emitter's owner (local space)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FVector LocationOffset = FVector::ZeroVector;

    // Rotation offset to apply on spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FRotator RotationOffset = FRotator::ZeroRotator;

    // Optional delay (seconds) before this entry spawns after the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float OffsetDelay = 0.f;

    // Should we inherit the owner’s rotation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bInheritOwnerRotation = true;

    // Should this entry have gravity applied?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bDrop = false;

    // Should this entry immediately scroll with the player?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bScrollInstant = false;

    // Should this entry scroll with player when player reaches it?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    bool bScrollOnPlayerReach = false;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEmitterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

    // All spawn configurations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter")
    TArray<FSpawnEntry> SpawnEntries;

    // Called when the timer triggers
    UFUNCTION()
    void OnPerformAction();

    // Helper that actually spawns a given entry
    void SpawnActor(int32 Index);
		
};

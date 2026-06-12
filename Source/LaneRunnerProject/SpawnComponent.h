// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChunkInitializable.h"
#include "SpawnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API USpawnComponent : public UActorComponent, public IChunkInitializable
{
	GENERATED_BODY()

public:	
	USpawnComponent();
	virtual void TeardownFromChunk_Implementation() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CurrentSpawned;

	bool bIsPooledInstance;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Despawn();

	UFUNCTION(BlueprintCallable)
	void Spawn(bool drop, bool scrollInstant, bool scrollOnReach);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ResetAsSpawned = true;

	bool VariantPreventsSpawn = false;

	UFUNCTION(BlueprintCallable)
	void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnedCollisionTag = FName("GravAndOverlap");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DespawnedCollisionTag = FName("NoCollision");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnedCollisionTag_SightBlocker = FName("QueryOnly");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DespawnedCollisionTag_SightBlocker = FName("NoCollision");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnedCollisionTag_LineOfSight = FName("OverlapAllDynamic");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DespawnedCollisionTag_LineOfSight = FName("NoCollision");

	UPROPERTY(BlueprintAssignable)
	FSpawnEvent OnSpawn;

	UPROPERTY(BlueprintAssignable)
	FSpawnEvent OnDespawn;

	bool GetCurrentSpawned();
};

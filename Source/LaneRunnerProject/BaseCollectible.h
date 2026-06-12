// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ChunkInitializable.h"
#include "ECollectibleType.h"
#include "ChunkInitializable.h"
#include "BaseCollectible.generated.h"

UCLASS()
class LANERUNNERPROJECT_API ABaseCollectible : public AActor, public IChunkInitializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCollectible();
	virtual void InitializeFromChunk_Implementation() override;
	virtual void ResetOnChunkRequest_Implementation() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool Collected;
	FTimerHandle OverlapEnableTimer;

	bool bReadyToCollect;

public:
	bool GetIsCollected();

	void Collect();

	UFUNCTION()
	void ResetCollectible();

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECollectibleType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PointsValue = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GivePoints = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int HealValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool HealPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsPooledInstance = true;

	UFUNCTION()
	void CheckOverlapOnSpawn();

	UFUNCTION()
	void OnSpawn();
};

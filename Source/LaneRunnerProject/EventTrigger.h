// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GI_LevelSystem.h"
#include "LevelChunkActor.h"
#include "ChunkInitializable.h"
#include "EventTrigger.generated.h"

UCLASS()
class LANERUNNERPROJECT_API AEventTrigger : public AActor, public IChunkInitializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEventTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	TArray<FLevelEventData> EventsToTrigger;

	UFUNCTION(BlueprintCallable)
	void ResolveTargetActorIDs(ALevelChunkActor* parentChunk);

	UFUNCTION(BlueprintCallable)
	void TriggerEvents();
};

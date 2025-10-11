// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkSpawnEntry.h"
#include "LevelChunkConfigData.h"
#include "LevelChunkActor.generated.h"



UCLASS()
class LANERUNNERPROJECT_API ALevelChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChunkActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AActor* GetChildActorByID(FName childID, bool& success);

    UFUNCTION(BlueprintCallable)
    void SpawnChunkElements();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelChunkConfigData* ConfigData;

protected:
	TArray<AActor*> SpawnedActors;
};

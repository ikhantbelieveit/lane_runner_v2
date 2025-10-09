// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkInitializable.h"
#include "Bullseye.generated.h"

UCLASS()
class LANERUNNERPROJECT_API ABullseye : public AActor, public IChunkInitializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullseye();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry) override;
};

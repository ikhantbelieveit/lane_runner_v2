// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkInitializable.h"
#include "EProjectileDirection.h"
#include "BullseyeGroup.generated.h"

UCLASS()
class LANERUNNERPROJECT_API ABullseyeGroup : public AActor, public IChunkInitializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullseyeGroup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry) override;
	void Initialise();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	bool StartScrollEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	float ScrollXPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	bool StartSpawned = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	bool ScrollOnPlayerDetect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	EProjectileDirection AdvanceDirection;
};

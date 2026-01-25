// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkInitializable.h"
#include "EProjectileDirection.h"
#include "BaseEnemy.h"
#include "BullseyeGroup.generated.h"

USTRUCT(BlueprintType)
struct FWarningSignData
{
	GENERATED_BODY()

public:
	FVector SignPosition;
	ABaseEnemy* EnemyRef;
	ABullseyeGroup* GroupRef;
};

USTRUCT(BlueprintType)
struct FDynamicMoveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool ShouldStopOnscreen;

	//on objects that don't stop onscreen this is the only one they use
	UPROPERTY(EditAnywhere)
	FVector MoveFromSpawnStopCoords;

	UPROPERTY(EditAnywhere)
	FVector MoveToDespawnStopCoords;

	//on objects that don't stop onscreen this is the only one they use
	UPROPERTY(EditAnywhere)
	float MoveFromSpawnSpeed = 250.0f;	

	UPROPERTY(EditAnywhere)
	float MoveToDespawnSpeed = 700.0f;

	//on objects that don't stop onscreen this is the only one they use
	UPROPERTY(EditAnywhere)
	EProjectileDirection MoveFromSpawnDirection;

	UPROPERTY(EditAnywhere)
	EProjectileDirection MoveToDespawnDirection;
};

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

	TArray<TWeakObjectPtr<AActor>> GroupMemberActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry) override;
	virtual void InitialiseFromChunk_Implementation(const FVector& ParentChunkLocation) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GroupBehaviour")
	FDynamicMoveData DynamicMoveData;

	TArray<FWarningSignData> WarningSigns;
};

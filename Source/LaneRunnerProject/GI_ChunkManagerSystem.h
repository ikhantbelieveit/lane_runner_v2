// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelLayoutData.h"
#include "GI_ChunkManagerSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_ChunkManagerSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SpawnChunksFromLayoutData(FLevelLayoutData layoutData);

	UFUNCTION(BlueprintCallable)
	float GetChunkLength(ALevelChunkActor* chunkActor);

	UFUNCTION(BlueprintCallable)
	void ClearChunks();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	TArray<ALevelChunkActor*> ActiveChunkActors;
};

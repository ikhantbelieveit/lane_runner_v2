// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelChunkDefinitionAsset.h"
#include "GI_ChunkDefinitionLoadSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_ChunkDefinitionLoadSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TMap<FName, FLevelChunkDefinition> ChunkDef_LUT;
};

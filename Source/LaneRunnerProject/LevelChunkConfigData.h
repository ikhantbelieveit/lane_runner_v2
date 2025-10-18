// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ChunkSpawnEntry.h"
#include "LevelChunkConfigData.generated.h"


UCLASS()
class LANERUNNERPROJECT_API ULevelChunkConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChunkSpawnEntry> SpawnActorEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* LaneMaterial_1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* LaneMaterial_2;
};

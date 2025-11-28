// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EUIState.h"
#include "BaseUIScreen.h"
#include "GI_AudioSystem.h"
#include "LevelChunkDefinitionAsset.h"
#include "Projectile.h"
#include "ECollectibleType.h"
#include "BaseCollectible.h"
#include "GI_ConfigData.generated.h"

USTRUCT(BlueprintType)
struct FUIConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EUIState, TSubclassOf<UBaseUIScreen>> UIScreen_LUT;
};

USTRUCT(BlueprintType)
struct FChunkDefLoadConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ULevelChunkDefinitionAsset*> ChunkDefs;
};

UCLASS(BlueprintType)
class LANERUNNERPROJECT_API UChunkDefLoadConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FChunkDefLoadConfig ConfigData;
};

UCLASS(BlueprintType)
class LANERUNNERPROJECT_API UProjectileSystemConfigData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EProjectileType, TSubclassOf<class AProjectile>> ProjectileClass_LUT;
};

USTRUCT(BlueprintType)
struct FCollectiblePoolConfigEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ECollectibleType Type;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseCollectible> Class;

	UPROPERTY(EditAnywhere)
	int32 InitialPoolSize = 0;
};

UCLASS(BlueprintType)
class UCollectiblePoolConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FCollectiblePoolConfigEntry> Entries;
};

UCLASS()
class LANERUNNERPROJECT_API UGI_ConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FUIConfig UIConfig;

	UPROPERTY(EditAnywhere)
	UAudioConfigData* AudioConfig;

	UPROPERTY(EditAnywhere)
	UChunkDefLoadConfigAsset* ChunkLoadConfig;

	UPROPERTY(EditAnywhere)
	UCollectiblePoolConfigData* CollectiblePoolConfig;

	UPROPERTY(EditAnywhere)
	UProjectileSystemConfigData* ProjectileSystemConfig;
	
};

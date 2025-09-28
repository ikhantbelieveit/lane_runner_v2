// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseGameSystem.h"
#include "SystemConfigData.h"
#include "ECharacterType.h"
#include "GameInitConfigData.generated.h"

USTRUCT(BlueprintType)
struct FInitSystemData
{
	GENERATED_BODY()

public:

	EGameSystem Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABaseGameSystem> SystemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USystemConfigData* ConfigDataAsset;
};

UCLASS()
class LANERUNNERPROJECT_API UGameInitConfigData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<EGameSystem, FInitSystemData> SystemConfigMap;
	
};

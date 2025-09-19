// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EUIState.h"
#include "BaseUIScreen.h"
#include "GI_ConfigData.generated.h"

USTRUCT(BlueprintType)
struct FUIConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EUIState, TSubclassOf<UBaseUIScreen>> UIScreen_LUT;
};

UCLASS()
class LANERUNNERPROJECT_API UGI_ConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FUIConfig UIConfig;
	
};

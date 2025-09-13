// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SystemConfigData.h"
#include "Blueprint/UserWidget.h"
#include "BaseUIScreen.h"
#include "UISystemConfigData.generated.h"

USTRUCT(BlueprintType)
struct FUIAssets
{
	GENERATED_BODY()

public:

	// Input mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSubclassOf<UBaseUIScreen> WidgetAsset;
};

UCLASS(BlueprintType)
class LANERUNNERPROJECT_API UUISystemConfigData : public USystemConfigData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FUIAssets AssetConfig;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelThemeData.generated.h"

UENUM(BlueprintType)
enum class ELevelTheme : uint8
{
	None UMETA(DisplayName = "None"),
	Desert_Basic UMETA(DisplayName = "Desert Basic")
};

USTRUCT(BlueprintType)
struct LANERUNNERPROJECT_API FLevelThemeVisuals
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* LaneMaterial_Odd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* LaneMaterial_Even;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* SlopeMaterial_Odd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* SlopeMaterial_Even;
};

USTRUCT(BlueprintType)
struct LANERUNNERPROJECT_API FLevelThemeData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FLevelThemeVisuals VisualsData;
};

UCLASS()
class LANERUNNERPROJECT_API ULevelThemeDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ELevelTheme ThemeType;
	UPROPERTY(EditAnywhere)
	FLevelThemeData ThemeData;
};

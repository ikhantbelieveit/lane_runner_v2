// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PaperFlipbook.h"
#include "ECharacterType.h"
#include "PlayerCharacterData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerVisualsData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* StandingFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpingFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpStartFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpLandFlipbook;
};

USTRUCT(BlueprintType)
struct FPlayerDisplayData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText PlayerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText FlavourText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* InfoPanelPortrait;
};

USTRUCT(BlueprintType)
struct FPlayerCharacterDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECharacterType CharacterType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerVisualsData VisualsData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerDisplayData DisplayData;
};

UCLASS()
class LANERUNNERPROJECT_API UPlayerCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerCharacterDefinition Definition;
};

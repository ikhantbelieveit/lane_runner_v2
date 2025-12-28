// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	None UMETA(DisplayName = "None"),
	Cowboy_Red UMETA(DisplayName = "Red Cowboy"),
	Cowboy_Purple UMETA(DisplayName = "Purple Cowboy"),
	Scrooge UMETA(DisplayName = "Scrooge"),
	Cow UMETA(DisplayName = "Cow"),
	Egg UMETA(DisplayName = "Egg Person"),
	Postman UMETA(DisplayName = "Postman"),
	Rat UMETA(DisplayName = "Rat"),
	Wonka UMETA(DisplayName = "Wonka"),
	ScroogeWife UMETA(DisplayName = "ScroogeWife")
};
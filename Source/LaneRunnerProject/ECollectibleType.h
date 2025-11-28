// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECollectibleType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECollectibleType : uint8
{
	None UMETA(DisplayName = "None"),
	Star_Basic UMETA(DisplayName = "Star Basic"),
	Star_Big UMETA(DisplayName = "Star Big")
};
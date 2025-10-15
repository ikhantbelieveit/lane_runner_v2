// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELevelChunkType : uint8
{
	None    UMETA(DisplayName = "None"),
	Start UMETA(DisplayName = "Start"),
	General UMETA(DisplayName = "General")
}; 
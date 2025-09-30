// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ELevelEventType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELevelEventType : uint8
{
	None    UMETA(DisplayName = "None"),
	TogglePlayerScroll    UMETA(DisplayName = "Toggle Player Scroll"),
	SetObjectPath    UMETA(DisplayName = "Set Object Path"),
	SetObjectSpeed    UMETA(DisplayName = "Set Object Speed"),
	TogglePlatform   UMETA(DisplayName = "Toggle Platform"),
	TogglePathFollow UMETA(DisplayName = "Toggle Path Follow")
};
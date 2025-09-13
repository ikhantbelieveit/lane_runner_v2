// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUIState.generated.h"

UENUM(BlueprintType)
enum class EUIState : uint8
{
	None       UMETA(DisplayName = "None"),
	TestLevel    UMETA(DisplayName = "TestLevel"),
	DeathScreen    UMETA(DisplayName = "DeathScreen"),
};
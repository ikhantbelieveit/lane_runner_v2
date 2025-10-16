// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterAnimState : uint8
{
	Grounded UMETA(DisplayName = "Grounded"),
	JumpStart UMETA(DisplayName = "Jump Start"),
	JumpLoop UMETA(DisplayName = "Jump Loop"),
	JumpLand UMETA(DisplayName = "Jump Land")
};
#pragma once

#include "CoreMinimal.h"
#include "EGameState.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	Active UMETA(DisplayName = "Active"),
	Win UMETA(DisplayName = "Win"),
	Lose UMETA(DisplayName = "Lose"),
	Dormant UMETA(DisplayName = "Dormant")
};
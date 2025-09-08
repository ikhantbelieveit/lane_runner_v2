#pragma once

#include "CoreMinimal.h"
#include "EPlayerJumpState.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EPlayerJumpState : uint8
{
	Grounded UMETA(DisplayName = "Grounded"),
	Rise UMETA(DisplayName = "Rise"),
	Apex UMETA(DisplayName = "Apex"),
	Fall UMETA(DisplayName = "Fall")
};
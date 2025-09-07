#pragma once

#include "CoreMinimal.h"
#include "EPlayerSpeedState.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EPlayerSpeedState : uint8
{
    Default UMETA(DisplayName = "Default"),
    Fast UMETA(DisplayName = "Fast"),
    Slow UMETA(DisplayName = "Slow")
};
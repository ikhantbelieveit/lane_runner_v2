#pragma once

#include "CoreMinimal.h"
#include "EProjectileDirection.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EProjectileDirection : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Up UMETA(DisplayName = "Up"),
	Forward UMETA(DisplayName = "Forward")
};
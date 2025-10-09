#pragma once

#include "CoreMinimal.h"
#include "EGameState.generated.h"


UENUM(BlueprintType)
enum class EGameState : uint8
{
	Active UMETA(DisplayName = "Active"),
	Win UMETA(DisplayName = "Win"),
	Lose UMETA(DisplayName = "Lose"),
	Dormant UMETA(DisplayName = "Dormant"),
	AwaitContinue UMETA(DisplayName = "AwaitContinue"),	//level is halted after taking damage, needs player to press continue
	Paused UMETA(DisplayName = "Paused")
};
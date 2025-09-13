// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUISystem.h"
#include "DeathScreenUISystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API ADeathScreenUISystem : public ABaseUISystem
{
	GENERATED_BODY()

public:
	virtual EUIState GetUIState() override;
	
};

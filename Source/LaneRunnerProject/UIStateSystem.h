// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameSystem.h"
#include "BaseUIScreen.h"
#include "UIStateSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API AUIStateSystem : public ABaseGameSystem
{
	GENERATED_BODY()

public:
	void EnterScreen();	//pass in enum or string ID

	void HideScreen();

	void HideAllScreens();

private:
	TArray<UBaseUIScreen*> ActiveScreens;

	//void ApplyInputMode(UBaseUIScreen* Screen);
};

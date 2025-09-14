// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameSystem.h"
#include "BaseUIScreen.h"
#include "BaseUISystem.h"
#include "EUIState.h"
#include "UIStateSystem.generated.h"



UCLASS()
class LANERUNNERPROJECT_API AUIStateSystem : public ABaseGameSystem
{
	GENERATED_BODY()

public:
	void EnterScreen(EUIState newScreen);	//pass in enum or string ID

	void RegisterSystem(EUIState State, ABaseUISystem* System);

private:
	TArray<UBaseUIScreen*> ActiveScreens;

	EUIState CurrentUIState;
	EUIState PrevUIState;

	const ABaseUISystem* GetSystemRefForState(EUIState state) const;

	TMap<EUIState, ABaseUISystem*> UISystem_LUT;
};

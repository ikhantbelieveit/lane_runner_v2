// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BaseUIScreen.h"
#include "EUIState.h"
#include "GI_UIStateSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_UIStateSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool InitialiseWidget(TSubclassOf<UBaseUIScreen> widget, EUIState state);

	void EnterScreen(EUIState newScreen);	//pass in enum or string ID

	void RegisterWidget(EUIState State, UBaseUIScreen* widget);

	void PrintFocusedWidget();

	bool HasInitialisedFromConfig;

	TMap<EUIState, UBaseUIScreen*> Widgets_LUT;

	UFUNCTION(BlueprintCallable)
	void QuitGame();

private:
	UBaseUIScreen* ActiveWidget;

	void ShowScreen(UBaseUIScreen* screen);
	void HideScreen(UBaseUIScreen* screen);


	EUIState CurrentUIState;
	EUIState PrevUIState;

	const UBaseUIScreen* GetScreenForState(EUIState state) const;

	void ApplyInputMode(UBaseUIScreen* widget);

	FTimerHandle TickHandle;

	void TickSubsystem(float DeltaTime);

	bool InitialiseFromConfig();
	
};

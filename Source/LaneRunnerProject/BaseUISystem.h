// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameSystem.h"
#include "BaseUIScreen.h"
#include "UISystemConfigData.h"
#include "EUIState.h"
#include "BaseUISystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API ABaseUISystem : public ABaseGameSystem
{
	GENERATED_BODY()

public:
	virtual void InitialiseWidget();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ShowScreen();

	virtual void HideScreen();

	virtual void ApplyInputMode();

	TSubclassOf<UBaseUIScreen> UIWidget;

	UBaseUIScreen* WidgetInstance;

	virtual void SetupFromConfig(USystemConfigData* configData) override;

	virtual EUIState GetUIState();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UUISystemConfigData* ConfigData;

public:
	UFUNCTION()
	void OnAllSystemsSpawned();

	bool IsActiveUI;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MySaveGame.h"
#include "GI_SaveSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_SaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SaveGame(UMySaveGame* newSave);

    UFUNCTION(BlueprintCallable)
    UMySaveGame* LoadGame();

    UPROPERTY()
    UMySaveGame* CurrentSave;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    bool HasExistingSave();
};

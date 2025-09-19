//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "BaseGameSystem.h"
//#include "MySaveGame.h"
//#include "GameSaveSystem.generated.h"
//
///**
// * 
// */
//UCLASS()
//class LANERUNNERPROJECT_API AGameSaveSystem : public ABaseGameSystem
//{
//	GENERATED_BODY()
//	
//public:
//    UFUNCTION(BlueprintCallable)
//    void SaveGame(UMySaveGame* newSave);
//
//    UFUNCTION(BlueprintCallable)
//    UMySaveGame* LoadGame();
//
//    UPROPERTY()
//    UMySaveGame* CurrentSave;
//
//    virtual void InitialiseSystem() override;
//
//    bool HasExistingSave();
//};

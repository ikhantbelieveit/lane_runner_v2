//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "GameSaveSystem.h"
//#include "Kismet/GameplayStatics.h"
//
//void AGameSaveSystem::SaveGame(UMySaveGame* newSave)
//{
//    if (!newSave) return;
//
//    if (!CurrentSave)
//    {
//        CurrentSave = Cast<UMySaveGame>(
//            UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
//    }
//
//    CurrentSave->SaveVersion = newSave->SaveVersion;
//
//    CurrentSave->OptionsData = newSave->OptionsData;
//    CurrentSave->ProgressData = newSave->ProgressData;
//    CurrentSave->StatsData = newSave->StatsData;
//
//    // Save it to disk
//    UGameplayStatics::SaveGameToSlot(CurrentSave, TEXT("SaveSlot"), 0);
//}
//
//UMySaveGame* AGameSaveSystem::LoadGame()
//{
//    if (UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot"), 0))
//    {
//        UMySaveGame* LoadedGame = Cast<UMySaveGame>(
//            UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot"), 0));
//
//        if (LoadedGame)
//        {
//            return LoadedGame;
//        }
//    }
//
//	return nullptr;
//}
//
//void AGameSaveSystem::InitialiseSystem()
//{
//    Super::InitialiseSystem();
//
//    CurrentSave = LoadGame();
//
//    if (HasExistingSave())
//    {
//        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("loaded game"));
//    }
//    else
//    {
//        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("no game found - creating new"));
//        CurrentSave = Cast<UMySaveGame>(
//            UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
//    }
//}
//
//bool AGameSaveSystem::HasExistingSave()
//{
//    return CurrentSave != nullptr;
//}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelThemeDataSystem.h"
#include "MyGameInstance.h"
#include "PlayerCharacter.h"

void UGI_LevelThemeDataSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TickHandle,
            [this]()
            {
                TickSubsystem(0.016f); // Approx 60 FPS
            },
            0.016f,
            true
        );
    }
}

void UGI_LevelThemeDataSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }
    Super::Deinitialize();
}

void UGI_LevelThemeDataSystem::SetLevelTheme(ELevelTheme newType)
{
    CurrentTheme = newType;
}

void UGI_LevelThemeDataSystem::TickSubsystem(float DeltaTime)
{
    if (!HasInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            HasInitialisedFromConfig = true;
        }
    }

}

bool UGI_LevelThemeDataSystem::InitialiseFromConfig()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (GI)
    {
        ULevelThemeDataSystemConfigData* configData = GI->ConfigData->LevelThemeDataSystemConfig;
        ThemeData_LUT.Empty();

        for (const TPair<ELevelTheme, ULevelThemeDefinition*> pair : configData->LevelThemeDefinitions)
        {
            ThemeData_LUT.Add(pair.Key, pair.Value->ThemeData);
        }
        return true;
    }

    return false;
}
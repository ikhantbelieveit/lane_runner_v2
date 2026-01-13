// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_CharacterDataSystem.h"
#include "MyGameInstance.h"
#include "PlayerCharacter.h"

void UGI_CharacterDataSystem::Initialize(FSubsystemCollectionBase& Collection)
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

void UGI_CharacterDataSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }
    Super::Deinitialize();
}

void UGI_CharacterDataSystem::SetCharacterType(ECharacterType newType)
{
    FPlayerCharacterDefinition def;

    if (!GetCharacterDefinition(newType, def)) { return; }

    CurrentCharacterType = newType;

    APlayerCharacter* playerActor = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
    if (playerActor)
    {
        playerActor->RefreshCharacterVisuals(def.VisualsData);
    }
}

void UGI_CharacterDataSystem::TickSubsystem(float DeltaTime)
{
    if (!HasInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            HasInitialisedFromConfig = true;
        }
    }

}

bool UGI_CharacterDataSystem::InitialiseFromConfig()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (GI)
    {
        UCharacterDataSystemConfigData* configData = GI->ConfigData->CharacterDataSystemConfig;
        CharacterDef_LUT.Empty();

        for (const TPair<ECharacterType, UPlayerCharacterData*> pair : configData->CharacterDefinitions)
        {
            CharacterDef_LUT.Add(pair.Key, pair.Value->Definition);
        }
        return true;
    }

    return false;
}
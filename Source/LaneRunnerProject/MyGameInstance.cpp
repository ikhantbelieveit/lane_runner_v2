// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "GI_AudioSystem.h"
#include "GI_UIStateSystem.h"

void UMyGameInstance::Init()
{
    Super::Init();

}

void UMyGameInstance::Shutdown()
{
    Super::Shutdown();

}

bool UMyGameInstance::AllSystemsReady()
{
    bool audioReady = false;
    bool uiReady = false;

    auto* uiSystem = GetSubsystem<UGI_UIStateSystem>();
    if (uiSystem)
    {
        uiReady = true;
    }

    auto* audioSystem = GetSubsystem<UGI_AudioSystem>();
    if (audioSystem)
    {
        audioReady = true;
    }

    if (audioReady && uiReady)
    {
        return true;
    }

    return false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UIStateSystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevelUISystem.h"
#include "DeathScreenUISystem.h"
#include "GameFramework/PlayerController.h"

void AUIStateSystem::EnterScreen(EUIState newScreen)
{
    if (CurrentUIState == newScreen)
    {
        return;
    }

    for (const TPair<EUIState, ABaseUISystem*> pair : UISystem_LUT)
    {
        EUIState checkState = pair.Key;
        ABaseUISystem* checkSystem = pair.Value;

        if (checkState != newScreen)
        {
            //hide
            checkSystem->HideScreen();
        }
        else
        {
            //show
            checkSystem->ShowScreen();
        }
    }

    PrevUIState = CurrentUIState;
    CurrentUIState = newScreen;
}

void AUIStateSystem::HideScreen()
{
    ABaseUISystem* system = GetSystemRefForState(EUIState::TestLevel);

    if (system)
    {
        system->HideScreen();
    }
}

void AUIStateSystem::HideAllScreens()
{
    /*for (UBaseUIScreen* Screen : ActiveScreens)
    {
        if (Screen)
        {
            Screen->OnScreenHidden();
            Screen->RemoveFromParent();
        }
    }
    ActiveScreens.Empty();

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }*/
}

void AUIStateSystem::RegisterSystem(EUIState state, ABaseUISystem* system)
{
    UISystem_LUT.Add(state, system);
}

ABaseUISystem* AUIStateSystem::GetSystemRefForState(EUIState state)
{
    switch (state)
    {
    case EUIState::DeathScreen:
    {
        ADeathScreenUISystem* uiSystem = Cast<ADeathScreenUISystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ADeathScreenUISystem::StaticClass()));
        if (uiSystem)
        {
            uiSystem->ShowScreen();
        }
    }
        
        break;
    case EUIState::TestLevel:
    {
        ATestLevelUISystem* uiSystem = Cast<ATestLevelUISystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ATestLevelUISystem::StaticClass()));
        if (uiSystem)
        {
            uiSystem->ShowScreen();
        }
    }
        
        break;
    default:
        return nullptr;
    }

    return nullptr;
}

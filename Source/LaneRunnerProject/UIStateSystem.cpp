// Fill out your copyright notice in the Description page of Project Settings.


#include "UIStateSystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevelUISystem.h"
#include "DeathScreenUISystem.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"

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

void AUIStateSystem::RegisterSystem(EUIState state, ABaseUISystem* system)
{
    UISystem_LUT.Add(state, system);
}

void AUIStateSystem::PrintFocusedWidget()
{
    TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0); // 0 = local player index
    if (FocusedWidget.IsValid())
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FocusedWidget->ToString());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("No widget currently focused."));
    }
}

void AUIStateSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    //PrintFocusedWidget();
}

const ABaseUISystem* AUIStateSystem::GetSystemRefForState(EUIState state) const
{
    if (const ABaseUISystem* const* FoundSystem = UISystem_LUT.Find(state))
    {
        return *FoundSystem;
    }
    return nullptr;
}

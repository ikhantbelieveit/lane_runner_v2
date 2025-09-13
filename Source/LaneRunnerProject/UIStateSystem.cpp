// Fill out your copyright notice in the Description page of Project Settings.


#include "UIStateSystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevelUISystem.h"
#include "GameFramework/PlayerController.h"

void AUIStateSystem::EnterScreen()
{
    ATestLevelUISystem* uiSystem = Cast<ATestLevelUISystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ATestLevelUISystem::StaticClass()));
    if (uiSystem)
    {
        uiSystem->ShowScreen();
    }
}

void AUIStateSystem::HideScreen()
{
    ATestLevelUISystem* uiSystem = Cast<ATestLevelUISystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ATestLevelUISystem::StaticClass()));
    if (uiSystem)
    {
        uiSystem->HideScreen();
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

//void AUIStateSystem::ApplyInputMode(UBaseUIScreen* screen)
//{
//    if (!screen) return;
//
//    APlayerController* PC = UGameplayStatics::GetPlayerController(screen->GetWorld(), 0);
//    if (!PC) return;
//
//    switch (screen->ScreenInputMode)
//    {
//    case EScreenInputMode::None:
//        PC->SetInputMode(FInputModeGameOnly());
//        PC->bShowMouseCursor = false;
//        break;
//
//    case EScreenInputMode::UIOnly:
//        PC->SetInputMode(FInputModeUIOnly());
//        PC->bShowMouseCursor = true;
//        break;
//
//    case EScreenInputMode::GameAndUI:
//        PC->SetInputMode(FInputModeGameAndUI());
//        PC->bShowMouseCursor = true;
//        break;
//    }
//}

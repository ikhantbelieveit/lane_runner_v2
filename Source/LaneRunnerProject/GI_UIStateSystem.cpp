// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_UIStateSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevelUISystem.h"
#include "DeathScreenUISystem.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "MyGameInstance.h"

void UGI_UIStateSystem::Initialize(FSubsystemCollectionBase& Collection)
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

void UGI_UIStateSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }
    Super::Deinitialize();
}

bool UGI_UIStateSystem::InitialiseWidget(TSubclassOf<UBaseUIScreen> widget, EUIState state)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
    {
        return false;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        return false;
    }
    

    UBaseUIScreen* Screen = CreateWidget<UBaseUIScreen>(PC, widget);
    if (Screen)
    {
        Screen->AddToViewport();
        Screen->SetVisibility(ESlateVisibility::Hidden); // Hide initially

        Screen->Initialise();

        RegisterWidget(state, Screen);

        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, widget->GetName());

        return true;
    }

    return false;
}

void UGI_UIStateSystem::EnterScreen(EUIState newScreen)
{
    if (CurrentUIState == newScreen)
    {
        return;
    }

    for (const TPair<EUIState, UBaseUIScreen*>& pair : Widgets_LUT)
    {
        UBaseUIScreen* checkSystem = pair.Value;
        if (checkSystem)
        {
            HideScreen(checkSystem);
        }
    }

    if (UBaseUIScreen** Found = Widgets_LUT.Find(newScreen))
    {
        if (*Found)
        {
            ShowScreen(*Found);
        }
    }

    PrevUIState = CurrentUIState;
    CurrentUIState = newScreen;
}

void UGI_UIStateSystem::RegisterWidget(EUIState state, UBaseUIScreen* widget)
{
    Widgets_LUT.Add(state, widget);
}

void UGI_UIStateSystem::PrintFocusedWidget()
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


void UGI_UIStateSystem::QuitGame()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, true);
    }
}

void UGI_UIStateSystem::ApplyInputMode(EScreenInputMode inputMode)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    switch (inputMode)
    {
    case EScreenInputMode::None:
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
        break;

    case EScreenInputMode::UIOnly:
        PC->SetInputMode(FInputModeUIOnly());
        PC->bShowMouseCursor = true;
        break;

    case EScreenInputMode::GameAndUI:
        PC->SetInputMode(FInputModeGameAndUI());
        PC->bShowMouseCursor = true;
        break;
    }
}

void UGI_UIStateSystem::ShowScreen(UBaseUIScreen* screen)
{
    screen->SetupBeforeShow();

    screen->SetVisibility(ESlateVisibility::Visible);
    ApplyInputMode(screen);
    screen->OnScreenShown();

    if (screen->ScreenInputMode != EScreenInputMode::None &&
        screen->DefaultSelection)
    {
        screen->DefaultSelection->SetKeyboardFocus();
    }

    ActiveWidget = screen;
}

void UGI_UIStateSystem::HideScreen(UBaseUIScreen* screen)
{
    if (!screen)
    {
        return;
    }

    screen->SetupBeforeHide();

    screen->SetVisibility(ESlateVisibility::Hidden);
    screen->OnScreenHidden();


    
    // Reapply mode for last active screen, or reset
    ApplyInputMode(EScreenInputMode::None);

    if (ActiveWidget == screen)
    {
        ActiveWidget = nullptr;
    }
}

const UBaseUIScreen* UGI_UIStateSystem::GetScreenForState(EUIState state) const
{
    if (const UBaseUIScreen* const* FoundSystem = Widgets_LUT.Find(state))
    {
        return *FoundSystem;
    }
    return nullptr;
}


void UGI_UIStateSystem::ApplyInputMode(UBaseUIScreen* widget)
{
    if (!widget) return;

    ApplyInputMode(widget->ScreenInputMode);
}

void UGI_UIStateSystem::TickSubsystem(float DeltaTime)
{
    if (!HasInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            HasInitialisedFromConfig = true;
        }
    }

    if (ActiveWidget)
    {
        ActiveWidget->Tick(DeltaTime);
    }
}

bool UGI_UIStateSystem::InitialiseFromConfig()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (GI)
    {
        for (const TPair<EUIState, TSubclassOf<UBaseUIScreen>> pair : GI->ConfigData->UIConfig.UIScreen_LUT)
        {

            EUIState state = pair.Key;
            TSubclassOf<UBaseUIScreen> screen = pair.Value;

            if (!InitialiseWidget(screen, state))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

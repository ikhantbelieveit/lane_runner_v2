// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_UIStateSystem.h"
#include "Blueprint/UserWidget.h"
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

    /*UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (GI)
    {
        for (const TPair<EUIState, TSubclassOf<UBaseUIScreen>> pair : GI->ConfigData->UIConfig.UIScreen_LUT)
        {
            
            EUIState state = pair.Key;
            TSubclassOf<UBaseUIScreen> screen = pair.Value;

            InitialiseWidget(screen, state);
        }
    }*/

    /*for (const TPair<EUIState, TSubclassOf<UBaseUIScreen>> pair : UIScreen_LUT)
    {
        EUIState state = pair.Key;
        TSubclassOf<UBaseUIScreen> screen = pair.Value;

        InitialiseWidget(screen, state);
    }*/
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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("world dont exist"));
        return false;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("player controller dont exist"));
        return false;
    }
    

    UBaseUIScreen* Screen = CreateWidget<UBaseUIScreen>(PC, widget);
    if (Screen)
    {
        Screen->AddToViewport();
        Screen->SetVisibility(ESlateVisibility::Hidden); // Hide initially

        Screen->Initialise();

        RegisterWidget(state, Screen);

        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, widget->GetName());

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

    for (const TPair<EUIState, UBaseUIScreen*> pair : Widgets_LUT)
    {
        EUIState checkState = pair.Key;
        UBaseUIScreen* checkSystem = pair.Value;

        if (checkState != newScreen)
        {
            HideScreen(checkSystem);
            //hide
            //checkSystem->SetupBeforeHide();

            //IsActiveUI = false;
            //checkSystem->SetVisibility(ESlateVisibility::Hidden);
            //checkSystem->OnScreenHidden();


            //// Reapply mode for last active screen, or reset
            //APlayerController* PC = UGameplayStatics::GetPlayerController(checkSystem->GetWorld(), 0);
            //if (PC)
            //{
            //    PC->SetInputMode(FInputModeGameOnly());
            //    PC->bShowMouseCursor = false;
            //}
        }
        else
        {
            ShowScreen(checkSystem);
            //show
            //checkSystem->SetupBeforeShow();

            ////IsActiveUI = true;
            //checkSystem->SetVisibility(ESlateVisibility::Visible);
            //ApplyInputMode(checkSystem);
            //checkSystem->OnScreenShown();

            //if (checkSystem->ScreenInputMode != EScreenInputMode::None &&
            //    checkSystem->DefaultSelection)
            //{
            //    checkSystem->DefaultSelection->SetKeyboardFocus();
            //}
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
    screen->SetupBeforeHide();

    screen->SetVisibility(ESlateVisibility::Hidden);
    screen->OnScreenHidden();


    // Reapply mode for last active screen, or reset
    APlayerController* PC = UGameplayStatics::GetPlayerController(screen->GetWorld(), 0);
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }

    ActiveWidget = nullptr;
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

    APlayerController* PC = UGameplayStatics::GetPlayerController(widget->GetWorld(), 0);
    if (!PC) return;

    switch (widget->ScreenInputMode)
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
        TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);

        if (!FocusedWidget.IsValid() || FocusedWidget->GetTypeAsString() == TEXT("SViewport"))
        {
            if (ActiveWidget->DefaultSelection && ActiveWidget->DefaultSelection->TakeWidget()->SupportsKeyboardFocus())
            {
                ActiveWidget->DefaultSelection->SetKeyboardFocus();
            }
        }
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

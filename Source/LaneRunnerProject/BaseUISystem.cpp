// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUISystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameInit.h"

void ABaseUISystem::InitialiseWidget()
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    UBaseUIScreen* Screen = CreateWidget<UBaseUIScreen>(PC, UIWidget);
    if (Screen)
    {
        Screen->AddToViewport();
        Screen->SetVisibility(ESlateVisibility::Hidden); // Hide initially
        //ScreenInstances.Add(ScreenClass, Screen);
        WidgetInstance = Screen;
    }
}

void ABaseUISystem::BeginPlay()
{
    Super::BeginPlay();

    AGameInit* gameInit = Cast<AGameInit>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameInit::StaticClass()));
    if (gameInit)
    {
        UIWidget = gameInit->TestUIClass;    //TODO: assign UI Widget BP in config data
    }

    InitialiseWidget();
}

void ABaseUISystem::ShowScreen()
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
    {
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        return;
    }

    //UBaseUIScreen* Screen = CreateWidget<UBaseUIScreen>(PC, UIWidget);
    if (WidgetInstance)
    {
        WidgetInstance->SetVisibility(ESlateVisibility::Visible);
        ApplyInputMode();
        WidgetInstance->OnScreenShown();
    }
}

void ABaseUISystem::HideScreen()
{
    if (WidgetInstance)
    {
        WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        WidgetInstance->OnScreenHidden();
        

        // Reapply mode for last active screen, or reset
        APlayerController* PC = UGameplayStatics::GetPlayerController(UIWidget->GetWorld(), 0);
        if (PC)
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
    }
}

void ABaseUISystem::ApplyInputMode()
{
    if (!WidgetInstance) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(WidgetInstance->GetWorld(), 0);
    if (!PC) return;

    switch (WidgetInstance->ScreenInputMode)
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

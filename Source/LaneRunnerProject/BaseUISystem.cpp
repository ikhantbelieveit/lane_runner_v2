//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BaseUISystem.h"
//#include "Kismet/GameplayStatics.h"
//#include "UIStateSystem.h"
//#include "EUIState.h"
//#include "BaseUIScreen.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"
//#include "Components/Widget.h"
//#include "Blueprint/UserWidget.h"
//#include "GameInit.h"
//
//void ABaseUISystem::InitialiseWidget()
//{
//    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
//    if (!World) return;
//
//    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
//    if (!PC) return;
//
//    UBaseUIScreen* Screen = CreateWidget<UBaseUIScreen>(PC, UIWidget);
//    if (Screen)
//    {
//        Screen->AddToViewport();
//        Screen->SetVisibility(ESlateVisibility::Hidden); // Hide initially
//        //ScreenInstances.Add(ScreenClass, Screen);
//        WidgetInstance = Screen;
//        WidgetInstance->Initialise();
//    }
//}
//
//void ABaseUISystem::BeginPlay()
//{
//    Super::BeginPlay();
//
//    AGameInit* gameInit = Cast<AGameInit>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameInit::StaticClass()));
//    if (gameInit)
//    {
//        gameInit->OnAllSystemsSpawned.AddDynamic(this, &ABaseUISystem::OnAllSystemsSpawned);
//    }
//
//}
//
//void ABaseUISystem::Tick(float DeltaTime)
//{
//    if (IsActiveUI)
//    {
//        TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);
//
//        if (!FocusedWidget.IsValid() || FocusedWidget->GetTypeAsString() == TEXT("SViewport"))
//        {
//            if (WidgetInstance->DefaultSelection && WidgetInstance->DefaultSelection->TakeWidget()->SupportsKeyboardFocus())
//            {
//                WidgetInstance->DefaultSelection->SetKeyboardFocus();
//            }
//        }
//    }
//}
//
//void ABaseUISystem::ShowScreen()
//{
//    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
//    if (!World)
//    {
//        return;
//    }
//
//    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
//    if (!PC)
//    {
//        return;
//    }
//
//    if (WidgetInstance)
//    {
//        WidgetInstance->SetupBeforeShow();
//
//        IsActiveUI = true;
//        WidgetInstance->SetVisibility(ESlateVisibility::Visible);
//        ApplyInputMode();
//        WidgetInstance->OnScreenShown();
//
//        if (WidgetInstance->ScreenInputMode != EScreenInputMode::None &&
//            WidgetInstance->DefaultSelection)
//        {
//            WidgetInstance->DefaultSelection->SetKeyboardFocus();
//        }
//    }
//}
//
//void ABaseUISystem::HideScreen()
//{
//    if (WidgetInstance)
//    {
//        WidgetInstance->SetupBeforeHide();
//
//        IsActiveUI = false;
//        WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
//        WidgetInstance->OnScreenHidden();
//        
//
//        // Reapply mode for last active screen, or reset
//        APlayerController* PC = UGameplayStatics::GetPlayerController(UIWidget->GetWorld(), 0);
//        if (PC)
//        {
//            PC->SetInputMode(FInputModeGameOnly());
//            PC->bShowMouseCursor = false;
//        }
//    }
//}
//
//void ABaseUISystem::ApplyInputMode()
//{
//    if (!WidgetInstance) return;
//
//    APlayerController* PC = UGameplayStatics::GetPlayerController(WidgetInstance->GetWorld(), 0);
//    if (!PC) return;
//
//    switch (WidgetInstance->ScreenInputMode)
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
//
//void ABaseUISystem::SetupFromConfig(USystemConfigData* configData)
//{
//    if (UUISystemConfigData* uiConfigData = Cast<UUISystemConfigData>(configData))
//    {
//        UIWidget = uiConfigData->AssetConfig.WidgetAsset;
//        InitialiseWidget();
//    }
//}
//
//EUIState ABaseUISystem::GetUIState()
//{
//    return EUIState::None;
//}
//
//void ABaseUISystem::OnAllSystemsSpawned()
//{
//    if (UIWidget)
//    {
//        AUIStateSystem* uiSystem = Cast<AUIStateSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIStateSystem::StaticClass()));
//        if (uiSystem)
//        {
//            uiSystem->RegisterSystem(GetUIState(), this);
//        }
//    }
//}

// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GI_LevelSystem.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add the mapping context to the local player’s subsystem
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    SetupEnhancedInput();
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    SetupEnhancedInput();
}

void AMyPlayerController::SetupEnhancedInput()
{
    if (HasSetUpInput)
    {
        return;
    }

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (InputAction_Pause)
        {
            EnhancedInput->BindAction(InputAction_Pause, ETriggerEvent::Started, this, &AMyPlayerController::Input_PauseStart);
            HasSetUpInput = true;
        }
        else
        {
        }
    }
    else
    {
    }
}

void AMyPlayerController::Input_PauseStart(const FInputActionValue& Value)
{
    if (auto* LevelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
    {
        LevelSystem->TogglePause();
    }
}
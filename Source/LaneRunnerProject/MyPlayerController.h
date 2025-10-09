// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

protected:

    /** Input Mapping Context */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /** Pause input action */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> InputAction_Pause;

    bool HasSetUpInput;

private:
    void SetupEnhancedInput();
    void Input_PauseStart(const FInputActionValue& Value);
    //add ui specific input here later
};

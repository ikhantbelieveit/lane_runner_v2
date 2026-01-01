// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnComponent.h"
#include "Components/ActorComponent.h"
#include "TimedActionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimedActionEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UTimedActionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTimedActionComponent();

protected:
    virtual void BeginPlay() override;

    FTimerHandle ActionTimerHandle;

    virtual void PerformAction();

    USpawnComponent* SpawnComponent;

public:
    // Interval between actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    float IntervalSeconds;

    // Initial offset before the first action
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    float StartDelay;

    // Whether the action should loop indefinitely
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    bool bLooping;

    // Starts the timed action
    UFUNCTION(BlueprintCallable, Category = "Timed Action")
    void StartAction();

    // Stops the timed action
    UFUNCTION(BlueprintCallable, Category = "Timed Action")
    void StopAction();

    // Resets the timer and starts over
    UFUNCTION(BlueprintCallable, Category = "Timed Action")
    void ResetAction();

    // Override in Blueprints for custom behavior
    UFUNCTION(BlueprintImplementableEvent, Category = "Timed Action")
    void OnPerformCustomAction();

    UPROPERTY(BlueprintAssignable, Category = "Overlap")
    FTimedActionEvent PerformActionEvent;

    // Whether event starts immediately
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    bool StartOnPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    bool RandomStartDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    float RandomStartDelayMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timed Action")
    float RandomStartDelayMax = 5.0f;

    

    UFUNCTION()
    void OnSpawned();

    UFUNCTION()
    void OnDespawned();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "TimedActionComponent.h"

UTimedActionComponent::UTimedActionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    IntervalSeconds = 1.0f;
    bLooping = true;
}

void UTimedActionComponent::BeginPlay()
{
    Super::BeginPlay();

    if (RandomStartDelay)
    {
        StartDelay = FMath::RandRange(RandomStartDelayMin, RandomStartDelayMax);
    }

    SpawnComponent = GetOwner()->FindComponentByClass<USpawnComponent>();
    if (SpawnComponent)
    {
        SpawnComponent->OnSpawn.AddDynamic(this, &UTimedActionComponent::OnSpawned);
        SpawnComponent->OnDespawn.AddDynamic(this, &UTimedActionComponent::OnDespawned);
    }

    if (StartOnPlay && (!SpawnComponent || SpawnComponent->GetCurrentSpawned()))
    {
        StartAction();
    }
}

void UTimedActionComponent::PerformAction()
{
    //do stuff here per type
    PerformActionEvent.Broadcast();

    // If not looping, stop after one call
    if (!bLooping)
    {
        StopAction();
    }
}

void UTimedActionComponent::StartAction()
{
    if (IntervalSeconds > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ActionTimerHandle,
            this,
            &UTimedActionComponent::PerformAction,
            IntervalSeconds,
            bLooping,
            StartDelay
        );
    }
}

void UTimedActionComponent::StopAction()
{
    GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
}

void UTimedActionComponent::ResetAction()
{
    StopAction();
    StartAction();
}

void UTimedActionComponent::OnSpawned()
{
    if (StartOnSpawn)
    {
        StartAction();
    }
}

void UTimedActionComponent::OnDespawned()
{
    StopAction();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "EmitterComponent.h"
#include "TimedActionComponent.h"
#include "Engine/World.h"
#include "SpawnComponent.h"

UEmitterComponent::UEmitterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEmitterComponent::BeginPlay()
{
    Super::BeginPlay();

    // Auto-link to first found TimedActionComponent on same actor if not manually set
    UTimedActionComponent* timer = GetOwner()->FindComponentByClass<UTimedActionComponent>();

    if (timer)
    {
        timer->PerformActionEvent.AddDynamic(this, &UEmitterComponent::OnPerformAction);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EmitterComponent on %s has no linked TimedActionComponent."), *GetOwner()->GetName());
    }
}

void UEmitterComponent::OnPerformAction()
{
    for (int32 i = 0; i < SpawnEntries.Num(); ++i)
    {
        const FSpawnEntry& Entry = SpawnEntries[i];
        if (Entry.OffsetDelay > 0.f)
        {
            FTimerHandle Handle;
            GetWorld()->GetTimerManager().SetTimer(
                Handle,
                FTimerDelegate::CreateUObject(this, &UEmitterComponent::SpawnActor, i),
                Entry.OffsetDelay,
                false
            );
        }
        else
        {
            SpawnActor(i);
        }
    }
}

void UEmitterComponent::SpawnActor(int32 Index)
{
    if (!GetWorld() || !SpawnEntries.IsValidIndex(Index))
        return;

    const FSpawnEntry& Entry = SpawnEntries[Index];
    if (!Entry.ActorClass)
        return;

    const AActor* Owner = GetOwner();
    if (!Owner)
        return;

    const FRotator OwnerRot = Owner->GetActorRotation();
    const FVector SpawnLoc = Owner->GetActorTransform().TransformPosition(Entry.LocationOffset);
    const FRotator SpawnRot = Entry.bInheritOwnerRotation
        ? (OwnerRot + Entry.RotationOffset)
        : Entry.RotationOffset;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Params.Owner = const_cast<AActor*>(Owner);

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Entry.ActorClass, SpawnLoc, SpawnRot, Params);
    if (!SpawnedActor)
        return;

    // Check for a SpawnComponent and call Spawn() if it exists
    USpawnComponent* SpawnComp = SpawnedActor->FindComponentByClass<USpawnComponent>();
    if (SpawnComp)
    {
        SpawnComp->Spawn(Entry.bDrop, Entry.bScrollInstant, Entry.bScrollOnPlayerReach);
    }
}
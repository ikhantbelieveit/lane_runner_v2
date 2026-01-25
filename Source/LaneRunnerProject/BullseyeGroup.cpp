// Fill out your copyright notice in the Description page of Project Settings.


#include "BullseyeGroup.h"
#include "Bullseye.h"
#include "LocationManagerComponent.h"
#include "SpawnComponent.h"
#include "PlayerDetectComponent.h"

// Sets default values
ABullseyeGroup::ABullseyeGroup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABullseyeGroup::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullseyeGroup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullseyeGroup::InitialiseFromChunk_Implementation()
{
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);

    ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>();
    UPlayerDetectComponent* detectComp = FindComponentByClass<UPlayerDetectComponent>();

    if (locManager)
    {
        locManager->bScrollEnabled = StartScrollEnabled;
        locManager->bStartScrollActive = StartScrollEnabled;
        locManager->ScrollWithXPos = ScrollXPos;
    }

    if (ScrollOnPlayerDetect)
    {
        if (detectComp)
        {
            FLevelEventData TriggerEvent;
            TriggerEvent.EventType = ELevelEventType::TogglePlayerScroll;
            TriggerEvent.BoolParam = true;
            TriggerEvent.TargetActors.Add(this);
            detectComp->EventsToTrigger.Add(TriggerEvent);
        }
    }

    for (AActor* Child : AttachedActors)
    {
        IGroupMemberInterface::Execute_OnAddedToGroup(Child, this, locManager);

        USpawnComponent* childSpawnComp = Child->FindComponentByClass<USpawnComponent>();
        if (childSpawnComp)
        {
            childSpawnComp->ResetAsSpawned = StartSpawned;
            if (!StartSpawned)
            {
                childSpawnComp->Despawn();
            }
        }
    }
}

void ABullseyeGroup::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{

}
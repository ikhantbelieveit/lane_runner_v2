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
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABullseyeGroup::BeginPlay()
{
	Super::BeginPlay();
	
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);

    if (ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>())
    {
        for (AActor* Child : AttachedActors)
        {
            if (Child->GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))
            {
                IGroupMemberInterface::Execute_OnAddedToGroup(Child, this, locManager);
            }
        }
    }
}

// Called every frame
void ABullseyeGroup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullseyeGroup::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{
    UE_LOG(LogTemp, Warning, TEXT("[BULLSEYE GROUP] Metadata: %s"), *Entry.Metadata);

    if (Entry.Metadata.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BULLSEYE GROUP] No metadata, aborting."));
        return;
    }

    // Parse JSON for group-level config
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

    if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
    {
        UE_LOG(LogTemp, Error, TEXT("[BULLSEYE GROUP] Invalid JSON"));
        return;
    }

    // Example group-only settings
    bool bScrollOnPlayerDetect = JsonObject->HasField(TEXT("ScrollOnPlayerDetect"))
        ? JsonObject->GetBoolField(TEXT("ScrollOnPlayerDetect"))
        : false;

    // Apply group-level settings to children
    TArray<AActor*> ChildActors;
    GetAllChildActors(ChildActors, true);

    for (AActor* Child : ChildActors)
    {
        if (!Child) continue;

        // --- NEW: Let the child handle its own chunk init ---------------
        if (Child->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
        {
            IChunkInitializable::Execute_InitializeFromChunkData(Child, Entry);
        }

        // --- Only do group-specific logic here --------------------------
        if (bScrollOnPlayerDetect)
        {
            if (UPlayerDetectComponent* DetectComp = Child->FindComponentByClass<UPlayerDetectComponent>())
            {
                FLevelEventData TriggerEvent;
                TriggerEvent.EventType = ELevelEventType::TogglePlayerScroll;
                TriggerEvent.BoolParam = true;
                TriggerEvent.TargetActors.Add(this);
                DetectComp->EventsToTrigger.Add(TriggerEvent);
            }
        }
    }
}
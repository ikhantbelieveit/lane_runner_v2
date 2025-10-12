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
    if (Entry.Metadata.IsEmpty()) return;


    // Parse JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        bool bScrollEnabled = false;
        if (JsonObject->HasField(TEXT("ScrollEnabled")))
        {
            bScrollEnabled = JsonObject->GetBoolField(TEXT("ScrollEnabled"));

        }


        float ScrollXPos = 0.0f;
        if (JsonObject->HasField(TEXT("ScrollXPos")))
        {
            ScrollXPos = JsonObject->GetNumberField(TEXT("ScrollXPos"));
        }

        bool bStartSpawned = false;
        if (JsonObject->HasField(TEXT("StartSpawned")))
        {
            bStartSpawned = JsonObject->GetBoolField(TEXT("StartSpawned"));
        }

        bool bScrollOnPlayerDetect = false;
        if (JsonObject->HasField(TEXT("ScrollOnPlayerDetect")))
        {
            bScrollOnPlayerDetect = JsonObject->GetBoolField(TEXT("ScrollOnPlayerDetect"));
        }

        TArray<AActor*> childActors;

        GetAllChildActors(childActors, true);

        for (AActor* child : childActors)
        {
            if (!child) continue;

            

            if (USpawnComponent* spawnComp = child->FindComponentByClass<USpawnComponent>())
            {
                spawnComp->ResetAsSpawned = bStartSpawned;
            }

            if (bScrollOnPlayerDetect)
            {
                if (UPlayerDetectComponent* detectComp = child->FindComponentByClass<UPlayerDetectComponent>())
                {
                    FLevelEventData triggerScrollEvent = FLevelEventData();
                    triggerScrollEvent.EventType = ELevelEventType::TogglePlayerScroll;
                    triggerScrollEvent.BoolParam = true;
                    triggerScrollEvent.TargetActors.Add(this);
                    detectComp->EventsToTrigger.Add(triggerScrollEvent);
                }
            }
        }

        if (ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>())
        {
            locManager->bStartScrollActive = bScrollEnabled;
            locManager->bScrollEnabled = bScrollEnabled;
            locManager->ScrollWithXPos = ScrollXPos;
        }
    }
}
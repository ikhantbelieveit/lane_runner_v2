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

void ABullseyeGroup::Initialise()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("CHUNK DATA INIT"));
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
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("ScrollOnPlayerDetect"));
        if (detectComp)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("detectComp"));
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
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Start Despawned"));
                childSpawnComp->bTrackDebugTEMP = true;
                childSpawnComp->Despawn();
            }
        }
    }
}

void ABullseyeGroup::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{
    /*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("CHUNK DATA INIT"));
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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("ScrollOnPlayerDetect"));
        if (detectComp)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("detectComp"));
            FLevelEventData TriggerEvent;
            TriggerEvent.EventType = ELevelEventType::TogglePlayerScroll;
            TriggerEvent.BoolParam = true;
            TriggerEvent.TargetActors.Add(this);
            detectComp->EventsToTrigger.Add(TriggerEvent);
        }
    }

    for (AActor* Child : AttachedActors)
    {
        if (locManager) //do i need this condition? surely always true
        {
            if (Child->GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))   //likewise this
            {
                IGroupMemberInterface::Execute_OnAddedToGroup(Child, this, locManager);
            }
        }

        USpawnComponent* childSpawnComp = Child->FindComponentByClass<USpawnComponent>();
        if (childSpawnComp)
        {
            childSpawnComp->ResetAsSpawned = StartSpawned;
        }
        //ULocationManagerComponent* childLocManager = Child->FindComponentByClass<ULocationManagerComponent>();

        if (childLocManager)
        {
            childLocManager->bScrollEnabled = StartScrollEnabled;
            childLocManager->bStartScrollActive = StartScrollEnabled;
            childLocManager->ScrollWithXPos = ScrollXPos;
        }
        
    }*/
    //UE_LOG(LogTemp, Warning, TEXT("[BULLSEYE GROUP] Metadata: %s"), *Entry.Metadata);

    //if (Entry.Metadata.IsEmpty())
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("[BULLSEYE GROUP] No metadata, aborting."));
    //    return;
    //}

    //// Parse JSON for group-level config
    //TSharedPtr<FJsonObject> JsonObject;
    //TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

    //if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
    //{
    //    UE_LOG(LogTemp, Error, TEXT("[BULLSEYE GROUP] Invalid JSON"));
    //    return;
    //}

    //// Example group-only settings
    //bool bScrollOnPlayerDetect = JsonObject->HasField(TEXT("ScrollOnPlayerDetect"))
    //    ? JsonObject->GetBoolField(TEXT("ScrollOnPlayerDetect"))
    //    : false;

    //// Apply group-level settings to children
    //TArray<AActor*> ChildActors;
    //GetAllChildActors(ChildActors, true);

    //for (AActor* Child : ChildActors)
    //{
    //    if (!Child) continue;

    //    // --- NEW: Let the child handle its own chunk init ---------------
    //    if (Child->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
    //    {
    //        IChunkInitializable::Execute_InitializeFromChunkData(Child, Entry);
    //    }

    //    // --- Only do group-specific logic here --------------------------
    //    if (bScrollOnPlayerDetect)
    //    {
    //        if (UPlayerDetectComponent* DetectComp = Child->FindComponentByClass<UPlayerDetectComponent>())
    //        {
    //            FLevelEventData TriggerEvent;
    //            TriggerEvent.EventType = ELevelEventType::TogglePlayerScroll;
    //            TriggerEvent.BoolParam = true;
    //            TriggerEvent.TargetActors.Add(this);
    //            DetectComp->EventsToTrigger.Add(TriggerEvent);
    //        }
    //    }
    //}
}
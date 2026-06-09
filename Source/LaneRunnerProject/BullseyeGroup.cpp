// Fill out your copyright notice in the Description page of Project Settings.


#include "BullseyeGroup.h"
#include "Bullseye.h"
#include "LocationManagerComponent.h"
#include "SpawnComponent.h"
#include "BaseEnemy.h"
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

void ABullseyeGroup::InitializeFromChunk_Implementation()
{
    GroupMemberActors.Empty();
    WarningSigns.Empty();

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
        GroupMemberActors.Add(Child);
        IGroupMemberInterface::Execute_OnAddedToGroup(Child, this, locManager);

        TArray<UActorComponent*> ChildActorComps;
        Child->GetComponents(ChildActorComps);
        for (UActorComponent* Comp : ChildActorComps)
        {
            if (USpawnComponent* spawnComp = Cast<USpawnComponent>(Comp))
            {
                spawnComp->ResetAsSpawned = StartSpawned;
                IChunkInitializable::Execute_InitializeFromChunk(spawnComp);
                if (!StartSpawned)
                {
                    UE_LOG(LogTemp, Log, TEXT("I want to despawn group member %s"), *Child->GetName());
                    spawnComp->Despawn();
                }
                continue;
            }
            if (Comp->Implements<UChunkInitializable>())
            {
                IChunkInitializable::Execute_InitializeFromChunk(Comp);
            }
        }

        if (ABaseEnemy* childEnemy = Cast<ABaseEnemy>(Child))
        {
            FWarningSignData signItemData;
            FVector signItemLocation = DynamicMoveData.MoveFromSpawnStopCoords + Child->GetActorTransform().GetRelativeTransform(GetActorTransform()).GetLocation();
            signItemData.SignPosition = signItemLocation;
            signItemData.EnemyRef = childEnemy;
            signItemData.GroupRef = this;
            WarningSigns.Add(signItemData);
        }
    }
}
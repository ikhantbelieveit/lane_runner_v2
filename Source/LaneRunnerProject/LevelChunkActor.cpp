// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChunkActor.h"
#include "EventTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkInitializable.h"


// Sets default values
ALevelChunkActor::ALevelChunkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelChunkActor::BeginPlay()
{
	Super::BeginPlay();
    SpawnChunkElements();


    for (AActor* Child : SpawnedActors)
    {
        // Only process if the child is (or inherits from) AEventTrigger
        AEventTrigger* EventTrigger = Cast<AEventTrigger>(Child);
        if (!EventTrigger)
            continue;

        // Resolve target actor references inside each EventTrigger
        EventTrigger->ResolveTargetActorIDs(this);

#if WITH_EDITOR
        // Optional sanity debug
        if (GEngine)
        {
            FString DebugMsg = FString::Printf(TEXT("Resolved EventTrigger: %s in chunk %s"),
                *EventTrigger->GetName(),
                *GetName());
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMsg);
        }
#endif
    }
}

void ALevelChunkActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    
}

void ALevelChunkActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    
}



// Called every frame
void ALevelChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* ALevelChunkActor::GetChildActorByID(FName childID, bool& success)
{
    success = false;

    if (childID.IsNone())
        return nullptr;

    for (AActor* Child : SpawnedActors)
    {
        if (!Child)
            continue;

        for (const FName& Tag : Child->Tags)
        {
            if (Tag == childID)
            {
                success = true;
                return Child;
            }
        }
    }

    return nullptr;
}

void ALevelChunkActor::SpawnChunkElements()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (AActor* oldActor : SpawnedActors)
    {
        oldActor->Destroy();
    }

    SpawnedActors.Empty();


    for (const FChunkSpawnEntry& Entry : ConfigData->SpawnActorEntries)
    {
        if (!Entry.ActorClass) continue;

        const FTransform SpawnTransform = Entry.RelativeTransform * GetActorTransform();
        AActor* NewActor = World->SpawnActorDeferred<AActor>(
            Entry.ActorClass,
            SpawnTransform,
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        if (NewActor)
        {
            // Assign tag or ID for lookups
            if (!Entry.ActorID.IsNone())
            {
                NewActor->Tags.Add(Entry.ActorID);
            }

            // Attach to chunk
            NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

            UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);

            // Optional: let actor read metadata
            if (NewActor->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
            {
                IChunkInitializable::Execute_InitializeFromChunkData(NewActor, Entry);
            }
            else
            {

            }

            SpawnedActors.Add(NewActor);
        }
    }
}


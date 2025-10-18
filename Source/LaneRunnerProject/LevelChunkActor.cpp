// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChunkActor.h"
#include "EventTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkInitializable.h"
#include "FloorComponent.h"


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
    
    if (ActiveVariants.Num() == 0 && ConfigData)
    {
        SpawnChunkElements();
    }

    // Apply to persistent actors in the scene (placed manually in the chunk blueprint)
    TArray<AActor*> ChildActors;
    GetAllChildActors(ChildActors, true);

    for (AActor* Child : ChildActors)
    {
        if (!Child) continue;

        // Deactivate if this actor doesn't match the selected variants
        if (!IsActorVariantActive(Child))
        {
            DeactivateActor(Child);
            continue;
        }

        // Initialize any floor components
        if (UFloorComponent* FloorComp = Child->FindComponentByClass<UFloorComponent>())
        {
            FloorComp->InitialiseFloor(ConfigData);
        }
    }

    // Resolve EventTriggers
    for (AActor* Child : SpawnedActors)
    {
        if (AEventTrigger* EventTrigger = Cast<AEventTrigger>(Child))
        {
            EventTrigger->ResolveTargetActorIDs(this);
        }
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
    if (!World || !ConfigData) return;

    // Clean up any previous actors
    for (AActor* OldActor : SpawnedActors)
    {
        if (OldActor)
            OldActor->Destroy();
    }
    SpawnedActors.Empty();

    for (const FChunkSpawnEntry& Entry : ConfigData->SpawnActorEntries)
    {
        if (!Entry.ActorClass) continue;

        FTransform SpawnTransform = Entry.RelativeTransform * GetActorTransform();

        AActor* NewActor = World->SpawnActorDeferred<AActor>(
            Entry.ActorClass,
            SpawnTransform,
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        if (!NewActor)
            continue;

        // Assign ID tag
        if (!Entry.ActorID.IsNone())
        {
            NewActor->Tags.Add(Entry.ActorID);
        }

        // Respect variant selection
        if (!IsActorVariantActive(NewActor))
        {
            DeactivateActor(NewActor);
        }

        // Preserve default scale if not overridden
        if (!Entry.bSetScale)
        {
            if (const AActor* DefaultActor = Cast<AActor>(Entry.ActorClass->GetDefaultObject()))
            {
                SpawnTransform.SetScale3D(DefaultActor->GetActorScale3D());
            }
        }

        NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);

        // Initialize via interface
        if (NewActor->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
        {
            IChunkInitializable::Execute_InitializeFromChunkData(NewActor, Entry);
        }

        SpawnedActors.Add(NewActor);
    }
}

bool ALevelChunkActor::IsActorVariantActive(const AActor* Actor) const
{
    if (!Actor) return true; // fail-safe

    for (const FName& Tag : Actor->Tags)
    {
        FString TagStr = Tag.ToString();

        if (TagStr.StartsWith("VAR_"))
        {
            // Remove VAR_ prefix
            FString Remainder = TagStr.RightChop(4); // 4 = length of "VAR_"

            FString SetName, OptionName;
            if (!Remainder.Split(TEXT("_"), &SetName, &OptionName))
            {
                UE_LOG(LogTemp, Warning, TEXT("Malformed variant tag: %s"), *TagStr);
                continue;
            }

            const FName* ActiveValue = ActiveVariants.Find(FName(*SetName));
            if (!ActiveValue)
            {
                UE_LOG(LogTemp, Warning, TEXT("No active variant entry for set: %s"), *SetName);
                return false;
            }

            if (*ActiveValue != FName(*OptionName))
            {
                return false; // this variant is not active
            }
        }
    }

    return true; // active if no VAR_ tag or matching variant
}

void ALevelChunkActor::DeactivateActor(AActor* Actor) const
{
    if (!Actor) return;
    Actor->SetActorHiddenInGame(true);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorTickEnabled(false);
}


void ALevelChunkActor::Teardown()
{
    for (AActor* Child : SpawnedActors)
    {
        if (!Child)
            continue;

        Child->Destroy();
    }
}

void ALevelChunkActor::RefreshForVariant()
{
    TArray<AActor*> ChildActors;
    GetAllChildActors(ChildActors, true);

    for (AActor* Child : ChildActors)
    {
        if (!Child) continue;

        // Skip if this is already in SpawnedActors (avoid double processing)
        if (SpawnedActors.Contains(Child)) continue;

        if (IsActorVariantActive(Child))
        {
            // Reactivate
            Child->SetActorHiddenInGame(false);
            Child->SetActorEnableCollision(true);
            Child->SetActorTickEnabled(true);

            // Initialize floor components
            if (UFloorComponent* FloorComp = Child->FindComponentByClass<UFloorComponent>())
            {
                FloorComp->InitialiseFloor(ConfigData);
            }
        }
        else
        {
            DeactivateActor(Child);
        }
    }


    SpawnChunkElements();
}


void ALevelChunkActor::InitializeFromLayoutData(const FLevelChunkData& InChunkData)
{
    ActiveVariants.Empty();
    

    for (const FChunkVariantEntry& Entry : InChunkData.ActiveVariants)
    {
        ActiveVariants.Add(FName(Entry.SetID), FName(Entry.Variant));
    }

    RefreshForVariant();

    // Debug print
    if (GEngine)
    {
        FString Msg = "[VARIANTS] ";
        for (auto& Pair : ActiveVariants)
        {
            Msg += Pair.Key.ToString() + "->" + Pair.Value.ToString() + " ";
        }
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }

    //SpawnChunkElements();
}

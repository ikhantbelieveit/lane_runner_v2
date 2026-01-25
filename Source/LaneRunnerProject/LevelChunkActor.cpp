// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChunkActor.h"
#include "EventTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkInitializable.h"
#include "FloorComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "BullseyeGroup.h"
#include "SpawnComponent.h"


// Sets default values
ALevelChunkActor::ALevelChunkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelChunkActor::BeginPlay()
{
	Super::BeginPlay();
    
    InitialiseBoundsBox();
    InitialiseActorID_LUT();
}

void ALevelChunkActor::InitialiseActorID_LUT()
{
    ActorID_LUT.Empty();

    TArray<AActor*> childActors;
    GetAttachedActors(childActors);

    for (AActor* child : childActors)
    {
        for (const FName& Tag : child->Tags)
        {
            FString TagStr = Tag.ToString();

            if (TagStr.StartsWith("ID_"))
            {
                FString Remainder = TagStr.RightChop(3);

                ActorID_LUT.Add(FName(Remainder), child);
            }
        }
    }
}

void ALevelChunkActor::InitialiseBoundsBox()
{
    BoundsBox = nullptr;

    TArray<UBoxComponent*> boxComps;
    GetComponents<UBoxComponent>(boxComps);

    for (UBoxComponent* Box : boxComps)
    {
        if (Box->ComponentHasTag("BoundsBox"))
        {
            BoundsBox = Box;
            break;
        }
    }

    if (!BoundsBox)
    {
        return;
    }

    BoundsBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelChunkActor::OnBoundsBoxBeginOverlap);
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

    if (ActorID_LUT.IsEmpty())  //TODO: remove condition once refactor complete
    {
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
    }

    else
    {
        AActor* foundChild = ActorID_LUT.Find(childID)->Get();
        if (foundChild)
        {
            success = true;
            return foundChild;
        }
    }

    return nullptr;
}

void ALevelChunkActor::SpawnChunkElements()
{
    UWorld* World = GetWorld();
    if (!World || !ConfigData) return;

    // Cleanup
    for (AActor* OldActor : SpawnedActors)
    {
        if (OldActor)
            OldActor->Destroy();
    }
    SpawnedActors.Empty();

    for (const FChunkSpawnEntry& Entry : ConfigData->SpawnActorEntries)
    {
        if (!Entry.ActorClass) continue;

        if (!Entry.VariantSet.IsNone() && !Entry.VariantOption.IsNone())
        {
            FName ActiveOption = ActiveVariants.FindRef(Entry.VariantSet);
            if (ActiveOption.IsValid() && ActiveOption != Entry.VariantOption)
            {
                continue; // skip this spawn entirely
            }
        }

        FTransform SpawnTransform = Entry.RelativeTransform * GetActorTransform();

        AActor* NewActor = World->SpawnActorDeferred<AActor>(
            Entry.ActorClass,
            SpawnTransform,
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        if (!NewActor) continue;

        // Assign ID tag
        if (!Entry.ActorID.IsNone())
        {
            NewActor->Tags.Add(Entry.ActorID);
        }

        // Assign variant tag if specified
        if (!Entry.VariantSet.IsNone() && !Entry.VariantOption.IsNone())
        {
            FString VariantTag = FString::Printf(TEXT("VAR_%s_%s"), *Entry.VariantSet.ToString(), *Entry.VariantOption.ToString());
            NewActor->Tags.Add(FName(*VariantTag));
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

        if (NewActor->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
        {
            //REMOVED HERE (eventually this will all get removed)
            //IChunkInitializable::Execute_InitializeFromChunkData(NewActor, Entry);
        }

        SpawnedActors.Add(NewActor);
    }

    for (AActor* Child : SpawnedActors)
    {
        if (AEventTrigger* EventTrigger = Cast<AEventTrigger>(Child))
        {
            EventTrigger->ResolveTargetActorIDs(this);
        }
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
    if (!Actor)
    {
        return;
    }
    if (!IsValid(Actor))
    {
        return;
    }

    Actor->SetActorEnableCollision(false);
    Actor->SetActorTickEnabled(false);

    USpawnComponent* spawnComp = Cast<USpawnComponent>(Actor->GetComponentByClass(USpawnComponent::StaticClass()));
    if (spawnComp)
    {
        spawnComp->VariantPreventsSpawn = true;
        spawnComp->Despawn();
    }
    else
    {
        Actor->SetActorHiddenInGame(true);
    }
}

void ALevelChunkActor::ReactivateActor(AActor* Actor) const
{
    if (!Actor)
    {
        return;
    }
    if (!IsValid(Actor))
    {
        return;
    }

    Actor->SetActorEnableCollision(true);
    Actor->SetActorTickEnabled(true);

    USpawnComponent* spawnComp = Cast<USpawnComponent>(Actor->GetComponentByClass(USpawnComponent::StaticClass()));
    if (spawnComp)
    {
        spawnComp->VariantPreventsSpawn = false;
        //spawning handled by reset callback later
    }
    else
    {
        Actor->SetActorHiddenInGame(false);
    }
}




void ALevelChunkActor::OnBoundsBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherComp) return;

    if (OtherComp->ComponentHasTag(TEXT("PlayerColl")))
    {
#pragma region DebugLogs
        FString debugMessage = FString::Printf(TEXT("[LEVEL] Player entered chunk bounds: %s"), *ChunkID.ToString());

        UE_LOG(LogTemp, Warning, TEXT("%s"), *debugMessage);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, debugMessage);
#pragma endregion
        
    }
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

void ALevelChunkActor::ApplyVariant()
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
            ReactivateActor(Child);

            UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(Child->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
            if (sprite)
            {
                sprite->SetVisibility(true);
            }

            UPaperFlipbookComponent* flipbook = Cast<UPaperFlipbookComponent>(Child->GetComponentByClass(UPaperFlipbookComponent::StaticClass()));
            if (flipbook)
            {
                flipbook->SetVisibility(true);
            }

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
    ChunkID = InChunkData.ChunkID;

    for (const TPair<FName, TWeakObjectPtr<AActor>>& pair : ActorID_LUT)
    {
        if (pair.Value->GetClass()->ImplementsInterface(UChunkInitializable::StaticClass()))
        {
            IChunkInitializable::Execute_InitialiseFromChunk(pair.Value.Get(), GetActorLocation());
        }

        if (AEventTrigger* EventTrigger = Cast<AEventTrigger>(pair.Value))
        {
            EventTrigger->ResolveTargetActorIDs(this);
        }
    }

    ActiveVariants.Empty();

    for (const FChunkVariantEntry& Entry : InChunkData.ActiveVariants)
    {
        ActiveVariants.Add(FName(Entry.SetID), FName(Entry.Variant));
    }

    ApplyVariant();

#pragma region DebugLogs
    //debug logs below
    /*if (ActiveVariants.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Chunk '%s' has no active variants."), *GetName());

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
                FString::Printf(TEXT("Chunk '%s' has no active variants!"), *GetName()));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("=== Active Variants for Chunk: %s ==="), *GetName());

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan,
                FString::Printf(TEXT("Active Variants for %s:"), *GetName()));

        for (const TPair<FName, FName>& Pair : ActiveVariants)
        {
            const FString SetName = Pair.Key.ToString();
            const FString VariantValue = Pair.Value.ToString();

            UE_LOG(LogTemp, Log, TEXT("  [%s] = %s"), *SetName, *VariantValue);

            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
                    FString::Printf(TEXT("  %s = %s"), *SetName, *VariantValue));
        }

        UE_LOG(LogTemp, Log, TEXT("====================================="));
    }*/
#pragma endregion
}

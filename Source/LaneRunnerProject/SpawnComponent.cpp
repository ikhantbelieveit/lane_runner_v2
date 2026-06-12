// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnComponent.h"
#include "GI_LevelSystem.h"
#include "GI_AudioSystem.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerDetectComponent.h"
#include "LocationManagerComponent.h"
#include "GI_CollectiblePoolSystem.h"
#include "BaseCollectible.h"

// Sets default values for this component's properties
USpawnComponent::USpawnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USpawnComponent::TeardownFromChunk_Implementation()
{
	Despawn();
}

// Called when the game starts
void USpawnComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USpawnComponent::Reset()
{
	if (ResetAsSpawned && !VariantPreventsSpawn)
	{
		Spawn(false, false, false);
	}
	else
	{
		Despawn();
	}
}

bool USpawnComponent::GetCurrentSpawned()
{
	return CurrentSpawned;
}


// Called every frame
void USpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USpawnComponent::Spawn(bool drop, bool scrollInstant, bool scrollOnReach)
{
	AActor* Owner = GetOwner();

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetVisibility(true);
	}

	TArray<UBoxComponent*> Boxes;
	Owner->GetComponents<UBoxComponent>(Boxes);

	for (UBoxComponent* box : Boxes)
	{
		if (!box) continue;

		box->SetGenerateOverlapEvents(true);

		if (box->ComponentHasTag("SightBox"))
		{
			box->SetCollisionProfileName(SpawnedCollisionTag_LineOfSight);
			continue;
		}

		if (box->ComponentHasTag("SightBlocker"))
		{
			box->SetCollisionProfileName(SpawnedCollisionTag_SightBlocker);
			continue;
		}

		box->SetCollisionProfileName(SpawnedCollisionTag);
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(Owner->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(true);
	}

	UPaperFlipbookComponent* flipbook = Cast<UPaperFlipbookComponent>(Owner->GetComponentByClass(UPaperFlipbookComponent::StaticClass()));
	if (flipbook)
	{
		flipbook->SetVisibility(true);
	}

	if (drop)
	{
		ULocationManagerComponent* locManager = Cast<ULocationManagerComponent>(Owner->GetComponentByClass(ULocationManagerComponent::StaticClass()));
		if (locManager)
		{
			locManager->SetGravityEnabled(true);
		}
	}

	if (scrollInstant)
	{
		ULocationManagerComponent* locManager = Cast<ULocationManagerComponent>(Owner->GetComponentByClass(ULocationManagerComponent::StaticClass()));
		if (locManager)
		{
			locManager->bScrollEnabled = true;
		}
	}

	if (scrollOnReach)
	{
		UPlayerDetectComponent* detectComp = Cast<UPlayerDetectComponent>(Owner->GetComponentByClass(UPlayerDetectComponent::StaticClass()));
		if (detectComp)
		{
			FLevelEventData NewEvent;
			NewEvent.EventType = ELevelEventType::TogglePlayerScroll;

			NewEvent.TargetActors.Add(Owner);

			NewEvent.BoolParam = true;
			detectComp->EventsToTrigger.Empty();
			detectComp->EventsToTrigger.Add(NewEvent);
		}
	}

	Owner->SetActorHiddenInGame(false);
	Owner->SetActorTickEnabled(true);

	CurrentSpawned = true;
	OnSpawn.Broadcast();
}

void USpawnComponent::Despawn()
{
	FString ownerName = GetOwner()->GetName();
	AActor* Owner = GetOwner();

	UE_LOG(LogTemp, Log, TEXT("despawn actor %s"), *ownerName);

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetVisibility(false);
	}

	TArray<UBoxComponent*> Boxes;
	Owner->GetComponents<UBoxComponent>(Boxes);

	for (UBoxComponent* box : Boxes)
	{
		if (!box) continue;

		if (box->ComponentHasTag("SightBox"))
		{
			box->SetCollisionProfileName(DespawnedCollisionTag_LineOfSight);
			continue;
		}

		if (box->ComponentHasTag("SightBlocker"))
		{
			box->SetCollisionProfileName(DespawnedCollisionTag_SightBlocker);
			continue;
		}
		box->SetCollisionProfileName(DespawnedCollisionTag);
		continue;
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(Owner->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(false);
	}

	UPaperFlipbookComponent* flipbook = Cast<UPaperFlipbookComponent>(GetOwner()->GetComponentByClass(UPaperFlipbookComponent::StaticClass()));
	if (flipbook)
	{
		flipbook->SetVisibility(false);
	}

	Owner->SetActorHiddenInGame(true);
	Owner->SetActorTickEnabled(false);

	CurrentSpawned = false;

	OnDespawn.Broadcast();
}
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
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpawnComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (!levelSystem)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (ABaseCollectible* Collectible = Cast<ABaseCollectible>(Owner))
	{
		if (Collectible->bIsPooledInstance)
		{
			return;
		}
	}

	levelSystem->CleanupBeforeReset.AddDynamic(this, &USpawnComponent::Reset);
	
}

void USpawnComponent::Reset()
{
	if (ResetAsSpawned)
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
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetVisibility(true);
	}

	TArray<UBoxComponent*> Boxes;
	GetOwner()->GetComponents<UBoxComponent>(Boxes);

	for (UBoxComponent* box : Boxes)
	{
		if (!box) continue;

		if (box->ComponentHasTag("IgnoreSpawnInit"))
		{
			continue;
		}

		if (box->ComponentHasTag("SightBlocker"))
		{
			box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			box->SetCollisionProfileName(SpawnedCollisionTag);
		}

		box->SetGenerateOverlapEvents(true);
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(true);
	}

	UPaperFlipbookComponent* flipbook = Cast<UPaperFlipbookComponent>(GetOwner()->GetComponentByClass(UPaperFlipbookComponent::StaticClass()));
	if (flipbook)
	{
		flipbook->SetVisibility(true);
	}

	if (drop)
	{
		ULocationManagerComponent* locManager = Cast<ULocationManagerComponent>(GetOwner()->GetComponentByClass(ULocationManagerComponent::StaticClass()));
		if (locManager)
		{
			locManager->SetGravityEnabled(true);
		}
	}

	if (scrollInstant)
	{
		ULocationManagerComponent* locManager = Cast<ULocationManagerComponent>(GetOwner()->GetComponentByClass(ULocationManagerComponent::StaticClass()));
		if (locManager)
		{
			locManager->bScrollEnabled = true;
		}

		
	}

	if (scrollOnReach)
	{
		UPlayerDetectComponent* detectComp = Cast<UPlayerDetectComponent>(GetOwner()->GetComponentByClass(UPlayerDetectComponent::StaticClass()));
		if (detectComp)
		{
			FLevelEventData NewEvent;
			NewEvent.EventType = ELevelEventType::TogglePlayerScroll;

			NewEvent.TargetActors.Add(GetOwner());

			NewEvent.BoolParam = true;
			detectComp->EventsToTrigger.Empty();
			detectComp->EventsToTrigger.Add(NewEvent);
		}
	}

	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->SetActorHiddenInGame(false);
		Owner->SetActorTickEnabled(true);
	}

	CurrentSpawned = true;

	OnSpawn.Broadcast();
}

void USpawnComponent::Despawn()
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetVisibility(false);
	}


	TArray<UBoxComponent*> Boxes;
	GetOwner()->GetComponents<UBoxComponent>(Boxes);

	for (UBoxComponent* box : Boxes)
	{
		if (!box) continue;

		box->SetCollisionProfileName(DespawnedCollisionTag);
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(false);
	}

	UPaperFlipbookComponent* flipbook = Cast<UPaperFlipbookComponent>(GetOwner()->GetComponentByClass(UPaperFlipbookComponent::StaticClass()));
	if (flipbook)
	{
		flipbook->SetVisibility(false);
	}

	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->SetActorHiddenInGame(true);
		Owner->SetActorTickEnabled(false);
	}

	CurrentSpawned = false;

	OnDespawn.Broadcast();
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnComponent.h"
#include "GI_LevelSystem.h"
#include "GI_AudioSystem.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerDetectComponent.h"
#include "LocationManagerComponent.h"

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
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &USpawnComponent::Reset);
	}
	
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

	UBoxComponent* box = Cast<UBoxComponent>(GetOwner()->GetComponentByClass(UBoxComponent::StaticClass()));
	if (box)
	{
		box->SetCollisionProfileName(SpawnedCollisionTag);
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(true);
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
}

void USpawnComponent::Despawn()
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetVisibility(false);
	}


	UBoxComponent* box = Cast<UBoxComponent>(GetOwner()->GetComponentByClass(UBoxComponent::StaticClass()));
	if (box)
	{
		//box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		box->SetCollisionProfileName(DespawnedCollisionTag);
	}

	UPaperSpriteComponent* sprite = Cast<UPaperSpriteComponent>(GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass()));
	if (sprite)
	{
		sprite->SetVisibility(false);
	}
}
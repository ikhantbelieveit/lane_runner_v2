// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObjectComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "GameInit.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GI_LevelSystem.h"
#include "LocationManagerComponent.h"
#include "DamageFlashComponent.h"
#include "SpawnComponent.h"
#include "GI_AudioSystem.h"
#include "GroupMember.h"
#include "DeathDummy.h"


// Sets default values for this component's properties
UDestructibleObjectComponent::UDestructibleObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDestructibleObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = StartHealth;

	UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		DefaultCollMode = box->GetCollisionEnabled();
	}

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UDestructibleObjectComponent::OnLevelReset);
	}
}


// Called every frame
void UDestructibleObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDestructibleObjectComponent::OnHit()
{
	if (GetIsDestroyed())
	{
		return;
	}

	CurrentHealth--;

	if (CurrentHealth <= 0)
	{
		DestroyFromComp();
	}

	if (FlashOnDamage)
	{
		UDamageFlashComponent* damageFlash = (UDamageFlashComponent*)GetOwner()->GetComponentByClass(UDamageFlashComponent::StaticClass());
		if (damageFlash)
		{
			damageFlash->TriggerFlash();
		}
	}
}

void UDestructibleObjectComponent::DestroyFromComp()
{
	if (GetIsDestroyed())
	{
		return;
	}

	if (GivePointsOnDestroy)
	{
		auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
		if (levelSystem)
		{
			if (levelSystem->GetGameState() == EGameState::Active)
			{
				levelSystem->AddToScore(PointsGivenOnDestroy);
			}
		}
	}

	USpawnComponent* spawnComp = Cast<USpawnComponent>(GetOwner()->GetComponentByClass(USpawnComponent::StaticClass()));
	if (spawnComp)
	{
		spawnComp->Despawn();
	}

	if (SpawnItemOnDestroy && SpawnCollectibleClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector spawnLoc = GetOwner()->GetActorLocation();
		spawnLoc.Y += FMath::RandRange(randomSpreadMin, randomSpreadMax);

		FRotator defaultRotation = FRotator();

		if (ABaseCollectible* item = GetWorld()->SpawnActor<ABaseCollectible>(SpawnCollectibleClass, spawnLoc, defaultRotation, SpawnParams))
		{
			if (USpawnComponent* itemSpawnComp = item->FindComponentByClass<USpawnComponent>())
			{
				itemSpawnComp->ResetAsSpawned = false;

				bool itemShouldScroll = false;

				ULocationManagerComponent* sourceLocManager = nullptr;
				AActor* owningActor = GetOwner();

				// --- Step 1: If THIS actor implements GroupMemberInterface ---
				if (GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))
				{
					sourceLocManager = IGroupMemberInterface::Execute_GetGroupManager(owningActor);
				}

				// --- Step 2: If owner implements GroupMemberInterface ---
				if (!sourceLocManager && owningActor && owningActor->GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))
				{
					sourceLocManager = IGroupMemberInterface::Execute_GetGroupManager(owningActor);
				}

				// --- Step 3: Fallback — look directly for a LocationManagerComponent on this actor ---
				if (!sourceLocManager)
				{
					sourceLocManager = owningActor->FindComponentByClass<ULocationManagerComponent>();
				}

				// --- Apply scroll settings if a location manager was found ---
				if (sourceLocManager)
				{
					if (ULocationManagerComponent* itemLocComp = item->FindComponentByClass<ULocationManagerComponent>())
					{
						itemShouldScroll = sourceLocManager->bScrollEnabled && sourceLocManager->ScrollWithXPos == 0.0f;
						itemLocComp->bScrollEnabled = itemShouldScroll;
					}
				}
				else
				{
					// No valid location manager found — spawn item without scrolling
					// UE_LOG(LogTemp, Verbose, TEXT("No location manager found for %s or its owner"), *GetName());
				}

				itemSpawnComp->Spawn(true, itemShouldScroll, true);
			}
		}
	}

	if (bSpawnDeathDummy)
	{
		if (DeathDummyClass)
		{
			FActorSpawnParameters Params;
			Params.Owner = GetOwner();
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLoc = GetOwner()->GetActorLocation();
			FRotator SpawnRot = FRotator::ZeroRotator;

			ADeathDummy* DeathDummy = GetWorld()->SpawnActor<ADeathDummy>(DeathDummyClass, SpawnLoc, SpawnRot, Params);
			if (DeathDummy)
			{
				// Optionally, pass info to it — like what sprite to display
				DeathDummy->InitFromActor(GetOwner());
			}
		}
	}
	
	Destroyed = true;

	OnDestroyed.Broadcast();
}

void UDestructibleObjectComponent::ResetDestroy()
{
	CurrentHealth = StartHealth;

	Destroyed = false;
}

void UDestructibleObjectComponent::OnLevelReset()
{
	ResetDestroy();
}

int UDestructibleObjectComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

bool UDestructibleObjectComponent::GetIsDestroyed()
{
	return Destroyed;
}

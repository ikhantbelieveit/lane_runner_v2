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
			levelSystem->AddToScore(PointsGivenOnDestroy);
		}
	}

	USpawnComponent* spawnComp = Cast<USpawnComponent>(GetOwner()->GetComponentByClass(USpawnComponent::StaticClass()));
	if (spawnComp)
	{
		spawnComp->Despawn();
	}

	if (SpawnItemOnDestroy)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FRotator defaultRotation = FRotator();

		if (ABaseCollectible* item = GetWorld()->SpawnActor<ABaseCollectible>(SpawnCollectibleClass, GetOwner()->GetActorLocation(), defaultRotation, SpawnParams))
		{
			if (IsValid(item))
			{
				ActiveCollectible = item;

				bool itemShouldScroll = false;

				ULocationManagerComponent* locationComp = Cast<ULocationManagerComponent>(GetOwner()->GetComponentByClass(ULocationManagerComponent::StaticClass()));
				ULocationManagerComponent* itemLocationComp = Cast<ULocationManagerComponent>(ActiveCollectible->GetComponentByClass(ULocationManagerComponent::StaticClass()));

				if (locationComp && itemLocationComp)
				{
					itemShouldScroll = locationComp->bScrollEnabled && locationComp->ScrollWithXPos == 0.0f;
				}

				USpawnComponent* itemSpawnComp = Cast<USpawnComponent>(ActiveCollectible->GetComponentByClass(USpawnComponent::StaticClass()));
				itemSpawnComp->ResetAsSpawned = false;

				FVector spawnLoc = GetOwner()->GetActorLocation();

				float spawnPosY = FMath::RandRange(randomSpreadMin, randomSpreadMax);
				spawnLoc.Y += spawnPosY;

				ActiveCollectible->SetActorLocation(spawnLoc);

				if (itemSpawnComp)
				{
					itemSpawnComp->Spawn(true, itemShouldScroll, true);
				}
			}			
		}
	}
	
	Destroyed = true;
}

void UDestructibleObjectComponent::ResetDestroy()
{
	CurrentHealth = StartHealth;

	if (Destroyed)
	{
		if (ActiveCollectible &&
			IsValid(ActiveCollectible))
		{
			ActiveCollectible->Destroy();
			ActiveCollectible = nullptr;
		}
	}

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

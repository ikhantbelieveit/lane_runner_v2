// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObjectComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "GameInit.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GI_LevelSystem.h"
#include "ScrollWithPlayerComponent.h"


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

	if (SpawnItemOnDestroy)
	{
		FActorSpawnParameters SpawnParams;

		FRotator defaultRotation = FRotator();

		if (ABaseCollectible* item = GetWorld()->SpawnActor<ABaseCollectible>(SpawnCollectibleClass, GetOwner()->GetActorLocation(), defaultRotation, SpawnParams))
		{
			item->ResetAsSpawned = false;
			item->Despawn();
			ActiveCollectible = item;
		}
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

	UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (mesh)
	{
		mesh->SetVisibility(false);
	}
	

	UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass());
	if (sprite)
	{
		sprite->SetVisibility(false);
	}

	if (ActiveCollectible)
	{
		bool itemShouldScroll = false;

		UScrollWithPlayerComponent* scrollComp = GetOwner()->GetComponentByClass<UScrollWithPlayerComponent>();
		UScrollWithPlayerComponent* itemScrollComp = ActiveCollectible->GetComponentByClass<UScrollWithPlayerComponent>();

		if (scrollComp && itemScrollComp)
		{
			itemShouldScroll = scrollComp->Enabled && scrollComp->ScrollWithXPos == 0.0f;
		}
		
		ActiveCollectible->Spawn();
		ActiveCollectible->SetActorLocation(GetOwner()->GetActorLocation());
		ActiveCollectible->SetGravityEnabled(true);
		if (itemShouldScroll)
		{
			itemScrollComp->ScrollWithXPos = 0.0f;
			itemScrollComp->Enabled = true;
		}
	}
	
	Destroyed = true;
}

void UDestructibleObjectComponent::ResetDestroy()
{
	CurrentHealth = StartHealth;

	if (Destroyed)
	{
		UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass());
		if (mesh)
		{
			mesh->SetVisibility(true);
		}


		UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
		if (box)
		{
			box->SetCollisionEnabled(DefaultCollMode);
		}


		UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass());
		if (sprite)
		{
			sprite->SetVisibility(true);
		}

		if (ActiveCollectible)
		{
			ActiveCollectible->Despawn();
		}
	}

	Destroyed = false;
}

void UDestructibleObjectComponent::OnLevelReset()
{
	ResetDestroy();

	UScrollWithPlayerComponent* itemScrollComp = ActiveCollectible->GetComponentByClass<UScrollWithPlayerComponent>();

	if (itemScrollComp)
	{
		itemScrollComp->Enabled = false;
	}
}

int UDestructibleObjectComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

bool UDestructibleObjectComponent::GetIsDestroyed()
{
	return Destroyed;
}

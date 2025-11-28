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
#include "GI_CollectiblePoolSystem.h"


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

	// Cache collision
	if (UBoxComponent* box = GetOwner()->FindComponentByClass<UBoxComponent>())
	{
		DefaultCollMode = box->GetCollisionEnabled();
	}

	// Listen for level reset
	if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
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
    if (Destroyed)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("IT IS ALREADY DESTROYED YOU FOOL"));
        return;
    }

    // Award points
    if (GivePointsOnDestroy)
    {
        if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
        {
            if (levelSystem->GetGameState() == EGameState::Active)
            {
                levelSystem->AddToScore(PointsGivenOnDestroy);
            }
        }
    }

    // Despawn this object
    if (USpawnComponent* spawnComp = GetOwner()->FindComponentByClass<USpawnComponent>())
    {
        spawnComp->Despawn();
    }

    if (SpawnItemOnDestroy)
    {
        bool itemShouldScroll = false;

        // Determine scroll inheritance
        ULocationManagerComponent* sourceLocManager = nullptr;
        AActor* owningActor = GetOwner();

        if (GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))
        {
            sourceLocManager = IGroupMemberInterface::Execute_GetGroupManager(owningActor);
        }
        if (!sourceLocManager && owningActor->GetClass()->ImplementsInterface(UGroupMemberInterface::StaticClass()))
        {
            sourceLocManager = IGroupMemberInterface::Execute_GetGroupManager(owningActor);
        }
        if (!sourceLocManager)
        {
            sourceLocManager = owningActor->FindComponentByClass<ULocationManagerComponent>();
        }

        if (sourceLocManager)
        {
            itemShouldScroll = sourceLocManager->bScrollEnabled &&
                sourceLocManager->ScrollWithXPos == 0.0f;
        }

        if (auto* Pool = GetWorld()->GetGameInstance()->GetSubsystem<UGI_CollectiblePoolSystem>())
        {
            FCollectibleRequest Req;
            Req.Type = SpawnItemType;
            Req.SpawnLocation = GetOwner()->GetActorLocation();
            Req.SpawnLocation.Y += FMath::RandRange(randomSpreadMin, randomSpreadMax);
            

            Req.bShouldScroll = itemShouldScroll;

            Pool->RequestCollectible(Req);
        }
    }


    // Spawn death dummy (unchanged)
    if (bSpawnDeathDummy && DeathDummyClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = GetOwner();
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector SpawnLoc = GetOwner()->GetActorLocation();
        ADeathDummy* DeathDummy = GetWorld()->SpawnActor<ADeathDummy>(
            DeathDummyClass, SpawnLoc, FRotator::ZeroRotator, Params);

        if (DeathDummy)
        {
            DeathDummy->InitFromActor(GetOwner());
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

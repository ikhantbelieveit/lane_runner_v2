// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCollectible.h"
#include "GI_LevelSystem.h"
#include "GI_AudioSystem.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerDetectComponent.h"
#include "LocationManagerComponent.h"
#include "SpawnComponent.h"
#include "GI_CollectiblePoolSystem.h"

// Sets default values
ABaseCollectible::ABaseCollectible()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCollectible::BeginPlay()
{
	Super::BeginPlay();

	bReadyToCollect = false;

	UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		box->OnComponentBeginOverlap.AddDynamic(this, &ABaseCollectible::HandleBeginOverlap);
	}

	auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &ABaseCollectible::OnLevelReset);
	}

	StartPos = GetActorLocation();

	auto* spawnComp = (USpawnComponent*)GetComponentByClass<USpawnComponent>();
	if (spawnComp)
	{
		spawnComp->OnSpawn.AddDynamic(this, &ABaseCollectible::OnSpawn);
		spawnComp->OnDespawn.AddDynamic(this, &ABaseCollectible::OnDespawn);
	}
}

void ABaseCollectible::OnSpawn()
{
	bReadyToCollect = true;
	Collected = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(OverlapEnableTimer, this, &ABaseCollectible::CheckOverlapOnInit, 0.2f, false);
	}
}

void ABaseCollectible::OnDespawn()
{
	ResetCollectible();

	if (bIsPooledInstance)
	{
		auto* Pool = GetWorld()->GetGameInstance()->GetSubsystem<UGI_CollectiblePoolSystem>();
		Pool->ReturnCollectible(this);
	}
}

void ABaseCollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCollectible::CheckOverlapOnInit()
{

	UBoxComponent* Box = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));

	TArray<UPrimitiveComponent*> Overlaps;
	Box->GetOverlappingComponents(Overlaps);

	for (UPrimitiveComponent* O : Overlaps)
	{
		if (!O) continue;

		if (O->ComponentHasTag("PlayerCollect"))
		{
			Collect();
			return;
		}
	}
}


bool ABaseCollectible::GetIsCollected()
{
	return Collected;
}

void ABaseCollectible::Collect()
{
	if (!bReadyToCollect)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("COLLECT FAIL - item not ready to collect"));
		return;
	}

	if (GetIsCollected())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("COLLECT FAIL - item already collected"));
		return;
	}

	if (GivePoints)
	{
		auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
		if (levelSystem)
		{
			if (levelSystem->GetGameState() == EGameState::Active)
			{
				levelSystem->AddToScore(PointsValue);
			}
		}
	}

	if (HealPlayer)
	{
		auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
		if (levelSystem)
		{
			if (levelSystem->GetGameState() == EGameState::Active)
			{
				levelSystem->HealPlayerFromItem(HealValue);
			}
		}
	}

	USpawnComponent* spawn = GetComponentByClass<USpawnComponent>();
	if (spawn)
	{
		spawn->Despawn();
	}

	Collected = true;

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	if (audioSystem)
	{
		audioSystem->Play(EAudioKey::CoinGet);
	}
}

void ABaseCollectible::ResetCollectible()
{
	Collected = false;
	bReadyToCollect = false;

	if (ULocationManagerComponent* LM = FindComponentByClass<ULocationManagerComponent>())
	{
		LM->bScrollEnabled = LM->bStartScrollActive;
	}
}

void ABaseCollectible::OnLevelReset()
{
	Collected = false;

}

void ABaseCollectible::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	FName PlayerName = "PlayerCollect";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		Collect();
	}
}
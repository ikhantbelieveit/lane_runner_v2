// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCollectible.h"
#include "GI_LevelSystem.h"
#include "GI_AudioSystem.h"
#include "PaperSpriteComponent.h"

// Sets default values
ABaseCollectible::ABaseCollectible()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCollectible::BeginPlay()
{
	Super::BeginPlay();

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
}

void ABaseCollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


bool ABaseCollectible::GetIsCollected()
{
	return Collected;
}

void ABaseCollectible::Collect()
{
	if (GetIsCollected())
	{
		return;
	}

	if (GivePoints)
	{
		auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
		if (levelSystem)
		{
			levelSystem->AddToScore(PointsValue);
		}
	}

	Despawn();

	Collected = true;

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	if (audioSystem)
	{
		audioSystem->Play(EAudioKey::CoinGet);
	}
}

void ABaseCollectible::OnLevelReset()
{
	if (ResetAsSpawned)
	{
		Spawn();
	}
	else
	{
		Despawn();
	}

	Collected = false;

	SetGravityEnabled(false);
}

void ABaseCollectible::ResetCollect()
{

	
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

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		Collect();
	}
}

void ABaseCollectible::Despawn()
{
	UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (mesh)
	{
		mesh->SetVisibility(false);
	}


	UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		//box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		box->SetCollisionProfileName("NoCollision");
	}

	UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetComponentByClass(UPaperSpriteComponent::StaticClass());
	if (sprite)
	{
		sprite->SetVisibility(false);
	}
}

void ABaseCollectible::Spawn()
{
	UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (mesh)
	{
		mesh->SetVisibility(true);
	}

	UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		//box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		box->SetCollisionProfileName("GravAndOverlap");
	}


	UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetComponentByClass(UPaperSpriteComponent::StaticClass());
	if (sprite)
	{
		sprite->SetVisibility(true);
	}
}

void ABaseCollectible::SetGravityEnabled(bool enabled)
{
	UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		box->SetEnableGravity(enabled);
	}
}
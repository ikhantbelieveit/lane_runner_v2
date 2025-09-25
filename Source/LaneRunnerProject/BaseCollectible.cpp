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
		DefaultCollMode = box->GetCollisionEnabled();

		box->OnComponentBeginOverlap.AddDynamic(this, &ABaseCollectible::HandleBeginOverlap);
	}

	auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &ABaseCollectible::OnLevelReset);
	}
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

	auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->AddToScore(10);
	}

	UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (mesh)
	{
		mesh->SetVisibility(false);
	}


	UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetComponentByClass(UPaperSpriteComponent::StaticClass());
	if (sprite)
	{
		sprite->SetVisibility(false);
	}

	Collected = true;

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	if (audioSystem)
	{
		audioSystem->Play(EAudioKey::CoinGet);
	}
}

void ABaseCollectible::OnLevelReset()
{
	ResetCollect();
}

void ABaseCollectible::ResetCollect()
{
	if (GetIsCollected())
	{
		UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetComponentByClass(UStaticMeshComponent::StaticClass());
		if (mesh)
		{
			mesh->SetVisibility(true);
		}


		UBoxComponent* box = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
		if (box)
		{
			box->SetCollisionEnabled(DefaultCollMode);
		}


		UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetComponentByClass(UPaperSpriteComponent::StaticClass());
		if (sprite)
		{
			sprite->SetVisibility(true);
		}
	}

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

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		Collect();
	}
}
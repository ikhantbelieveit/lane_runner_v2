// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "LineOfSightComponent.h"
#include "LocationManagerComponent.h"
#include "DestructibleObjectComponent.h"
#include "GI_AudioSystem.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	

	ULineOfSightComponent* lineOfSight = GetComponentByClass<ULineOfSightComponent>();
	if (lineOfSight)
	{
		lineOfSight->OnDetectPlayer.AddDynamic(this, &ABaseEnemy::OnDetectPlayer);
	}

	UPaperSpriteComponent* foundAlertSprite = nullptr;
	UPaperSpriteComponent* foundMainVisuals = nullptr;

	TArray<UPaperSpriteComponent*> spriteComps;

	GetComponents<UPaperSpriteComponent>(spriteComps);

	for (UPaperSpriteComponent* sprite : spriteComps)
	{
		if (!sprite)
		{
			continue;
		}

		if (sprite->ComponentHasTag("AlertSprite"))
		{
			foundAlertSprite = sprite;
		}

		if (sprite->ComponentHasTag("MainVisuals"))
		{
			foundMainVisuals = sprite;
		}
	}

	if (foundAlertSprite)
	{
		AlertVFX = foundAlertSprite;
		AlertVFX->SetVisibility(false);
	}

	if (foundMainVisuals)
	{
		MainVisuals = foundMainVisuals;
		MainVisuals->SetSprite(IdleSprite);
	}

	UDestructibleObjectComponent* foundDestructible = nullptr;

	TArray<UDestructibleObjectComponent*> destructibleComps;

	GetComponents<UDestructibleObjectComponent>(destructibleComps);

	for (UDestructibleObjectComponent* destructible : destructibleComps)
	{
		if (!destructible)
		{
			continue;
		}

		if (destructible->ComponentHasTag("MainDestructible"))
		{
			foundDestructible = destructible;
			break;
		}
	}

	if (foundDestructible)
	{
		foundDestructible->OnDestroyed.AddDynamic(this, &ABaseEnemy::OnKilled);
	}

	IsAlive = true;
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemy::OnDetectPlayer()
{
	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::StraightAdvance:
		if (IsAlive)
		{
			ULocationManagerComponent* locManager = GetComponentByClass<ULocationManagerComponent>();

			if (locManager)
			{
				locManager->StartAutoMove(EProjectileDirection::Backward, AdvanceSpeed, false, FVector::Zero(), false);
			}

			if (AlertVFX)
			{
				AlertVFX->SetVisibility(true);
			}

			if (MainVisuals)
			{
				MainVisuals->SetSprite(AlertSprite);
			}
		}
		
		break;
	}

	if (DetectBehaviour != EEnemyDetectBehaviour::None)
	{
		auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
		audioSystem->Play(EAudioKey::EnemyAlert);
	}
}

bool ABaseEnemy::IsEnemyAlive()
{
	return IsAlive;
}

void ABaseEnemy::OnKilled()
{
	IsAlive = false;

	if (AlertVFX)
	{
		AlertVFX->SetVisibility(false);
	}
}
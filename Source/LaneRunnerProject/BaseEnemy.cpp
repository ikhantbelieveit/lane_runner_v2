// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "LineOfSightComponent.h"
#include "LocationManagerComponent.h"
#include "DestructibleObjectComponent.h"
#include "GI_AudioSystem.h"
#include "GI_ProjectileSystem.h"
#include "GI_LevelSystem.h"

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

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &ABaseEnemy::OnLevelReset);
	}
}

bool ABaseEnemy::HasPerformedDetectAction()
{
	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::Shoot_OneOff:
		return PerformedOneOffShoot;
	case EEnemyDetectBehaviour::StraightAdvance:
		return PerformedAdvance;
	}
	return false;
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemy::OnLevelReset()
{
	PerformedOneOffShoot = false;
	PerformedAdvance = false;

	if (AlertVFX)
	{
		AlertVFX->SetVisibility(false);
	}

	if (MainVisuals)
	{
		MainVisuals->SetSprite(IdleSprite);
	}
}

void ABaseEnemy::OnDetectPlayer()
{
	if (!IsAlive)
	{
		return;
	}

	if (HasPerformedDetectAction())
	{
		return;
	}

	if (DetectBehaviour == EEnemyDetectBehaviour::None)
	{
		return;
	}

	ULocationManagerComponent* locManager = GetComponentByClass<ULocationManagerComponent>();

	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::StraightAdvance:
		if (locManager)
		{
			locManager->StartAutoMove(EProjectileDirection::Backward, AdvanceSpeed, false, FVector::Zero(), false);
			PerformedAdvance = true;
		}
		break;
	case EEnemyDetectBehaviour::Shoot_OneOff:
		auto* projSystem = GetGameInstance()->GetSubsystem<UGI_ProjectileSystem>();

		FProjectileRequestData projRequest = ProjectileData;
		for (FShootItem& item : projRequest.Items)
		{
			FVector shootPos = GetActorLocation();
			item.ShootPos = shootPos;
		}

		if (!projSystem->ProcessProjectileRequest(projRequest))
		{
			//complain here
		}
		else
		{
			PerformedOneOffShoot = true;
		}
		
		break;
	}

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	audioSystem->Play(EAudioKey::EnemyAlert);

	if (AlertVFX && ShowAlertVFX)
	{
		AlertVFX->SetVisibility(true);
	}

	if (MainVisuals)
	{
		MainVisuals->SetSprite(AlertSprite);
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
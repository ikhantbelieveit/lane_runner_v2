// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "LineOfSightComponent.h"
#include "LocationManagerComponent.h"
#include "DestructibleObjectComponent.h"

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
			break;
		}
	}

	if (foundAlertSprite)
	{
		AlertSprite = foundAlertSprite;
		AlertSprite->SetVisibility(false);
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

			if (AlertSprite)
			{
				AlertSprite->SetVisibility(true);
			}
		}
		
		break;
	}
}

bool ABaseEnemy::IsEnemyAlive()
{
	return IsAlive;
}

void ABaseEnemy::OnKilled()
{
	IsAlive = false;

	if (AlertSprite)
	{
		AlertSprite->SetVisibility(false);
	}
}
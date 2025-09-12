// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSystem.h"
#include "EngineUtils.h"
#include "LevelSystem.h"
#include "GameInit.h"

void AProjectileSystem::BeginPlay()
{
	Super::BeginPlay();

	AGameInit* gameInit = Cast<AGameInit>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameInit::StaticClass()));
	if (gameInit)
	{
		gameInit->OnAllSystemsSpawned.AddDynamic(this, &AProjectileSystem::RegisterGameSystemDelegates);
	}
}

bool AProjectileSystem::ShootPlayerProjectile(FProjectileRequestData request)
{
	bool success = false;

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FVector shootPos = request.ShootPos;
		FRotator defaultRotation = FRotator();

		AProjectile* Projectile = World->SpawnActor<AProjectile>(request.ProjectileClass, shootPos, defaultRotation, SpawnParams);
		if (Projectile)
		{
			Projectile->SetFiringDirection(request.Direction);
			Projectile->SetupFromConfig();
			Projectile->Fire(request.Direction);

			success = true;
		}
	}

	return success;
}

void AProjectileSystem::ClearAllProjectiles()
{
	for (TActorIterator<AProjectile> It(GetWorld()); It; ++It)
	{
		AProjectile* Proj = *It;
		if (Proj)
		{
			Proj->Destroy();
		}
	}
}

void AProjectileSystem::RegisterGameSystemDelegates()
{
	ALevelSystem* gameInit = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
	if (gameInit)
	{
		gameInit->CleanupBeforeReset.AddDynamic(this, &AProjectileSystem::OnLevelReset);
	}
}

void AProjectileSystem::OnLevelReset()
{
	ClearAllProjectiles();
}
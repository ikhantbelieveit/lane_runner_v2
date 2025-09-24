// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_ProjectileSystem.h"
#include "GI_LevelSystem.h"
#include "EngineUtils.h"

void UGI_ProjectileSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UGI_LevelSystem>();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UGI_ProjectileSystem::OnLevelReset);
	}

	UE_LOG(LogTemp, Log, TEXT("UI Manager Subsystem initialized after LevelSystemSubsystem"));
}

bool UGI_ProjectileSystem::ShootPlayerProjectile(FProjectileRequestData request)
{
	bool success = false;

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

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

void UGI_ProjectileSystem::ClearAllProjectiles()
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

void UGI_ProjectileSystem::OnLevelReset()
{
	ClearAllProjectiles();
}
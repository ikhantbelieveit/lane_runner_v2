// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_ProjectileSystem.h"
#include "GI_LevelSystem.h"
#include "EngineUtils.h"
#include "MyGameInstance.h"
#include "GI_ConfigData.h"

void UGI_ProjectileSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UGI_LevelSystem>();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UGI_ProjectileSystem::OnLevelReset);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickHandle,
			[this]()
			{
				TickSubsystem(0.016f); // Approx 60 FPS
			},
			0.016f,
			true
		);
	}
}

void UGI_ProjectileSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickHandle);
	}
	Super::Deinitialize();
}

void UGI_ProjectileSystem::TickSubsystem(float DeltaTime)
{
	if (!HasInitialisedFromConfig)
	{
		if (InitialiseFromConfig())
		{
			HasInitialisedFromConfig = true;
		}
	}

}

bool UGI_ProjectileSystem::InitialiseFromConfig()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI)
	{
		UProjectileSystemConfigData* configData = GI->ConfigData->ProjectileSystemConfig;
		ProjectileClass_LUT.Empty();

		for (const TPair<EProjectileType, TSubclassOf<AProjectile>> pair : configData->ProjectileClass_LUT)
		{
			ProjectileClass_LUT.Add(pair);
		}

		return true;
	}

	return false;
}

bool UGI_ProjectileSystem::ProcessProjectileRequest(FProjectileRequestData request)
{
	bool success = true;

	UWorld* World = GetWorld();
	if (World)
	{
		for (const FShootItem shootItem : request.Items)
		{
			FActorSpawnParameters SpawnParams;
			FVector shootPos = shootItem.ShootPos;
			FRotator defaultRotation = FRotator();

			TSubclassOf<class AProjectile>* projClass = ProjectileClass_LUT.Find(shootItem.Type);
			if (projClass)
			{
				AProjectile* Projectile = World->SpawnActor<AProjectile>(*projClass, shootPos, defaultRotation, SpawnParams);
				if (Projectile)
				{
					Projectile->SetFiringDirection(shootItem.Direction);
					Projectile->SetupFromConfig();
					Projectile->Fire(shootItem.Direction);

				}
			}
			else
			{
				success = false;
			}
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



// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_WarningSignSystem.h"
#include "GI_LevelSystem.h"
#include "EngineUtils.h"
#include "MyGameInstance.h"
#include "SpawnComponent.h"
#include "GI_ConfigData.h"

void UGI_WarningSignSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UGI_WarningSignSystem>();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UGI_WarningSignSystem::OnLevelReset);
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

void UGI_WarningSignSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickHandle);
	}
	Super::Deinitialize();
}

void UGI_WarningSignSystem::TickSubsystem(float DeltaTime)
{
	if (!HasInitialisedFromConfig)
	{
		if (InitialiseFromConfig())
		{
			HasInitialisedFromConfig = true;
		}
	}

}

bool UGI_WarningSignSystem::InitialiseFromConfig()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI)
	{
		UWarningSignSystemConfigData* configData = GI->ConfigData->WarningSignSystemConfig;
		WarningSignClass = configData->WarningSignClass;

		InitialisePools(configData->PrewarmSignCount);

		return true;
	}

	return false;
}

AWarningSign* UGI_WarningSignSystem::CreateNewWarningSign()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWarningSign* NewSign =
		World->SpawnActor<AWarningSign>(WarningSignClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

	if (NewSign)
	{
		//NewCol->bIsPooledInstance = true;

		if (USpawnComponent* SpawnComp = NewSign->FindComponentByClass<USpawnComponent>())
		{
			SpawnComp->ResetAsSpawned = false;
			SpawnComp->Despawn();
		}
	}

	return NewSign;
}

void UGI_WarningSignSystem::InitialisePools(int prewarmCount)
{
	for (int32 i = 0; i < prewarmCount; ++i)
	{
		AWarningSign* Sign = CreateNewWarningSign();
		if (Sign)
		{
			WarningSignPool.Inactive.Add(Sign);
		}
	}
}

void UGI_WarningSignSystem::ResetAllPools()
{
	for (auto& WeakSign : WarningSignPool.Active)
	{
		if (AWarningSign* Sign = WeakSign.Get())
		{
			Sign->ResetSign();
			WarningSignPool.Inactive.Add(Sign);
		}
	}

	WarningSignPool.Active.Empty();
}

void UGI_WarningSignSystem::OnLevelReset()
{
	ResetAllPools();
}

bool UGI_WarningSignSystem::RequestWarningSign(FWarningSignData signRequestData)
{
	bool success = false;

	for (int32 i = WarningSignPool.Inactive.Num() - 1; i >= 0; --i)
	{
		if (AWarningSign* Sign = WarningSignPool.Inactive[i].Get())
		{
			WarningSignPool.Inactive.Remove(Sign);
			WarningSignPool.Active.Add(Sign);

			Sign->SetActorLocation(signRequestData.SignPosition);

			if (USpawnComponent* SpawnComp = Sign->FindComponentByClass<USpawnComponent>())
			{
				SpawnComp->Spawn(false, true, false);
			}

			Sign->InitialiseSign(signRequestData);

			success = true;
			return success;
		}
		else
		{
			WarningSignPool.Inactive.RemoveAt(i);
		}
	}

	// None available, create new
	AWarningSign* NewSign = CreateNewWarningSign();
	if (NewSign)
	{
		WarningSignPool.Active.Add(NewSign);
		NewSign->SetActorLocation(signRequestData.SignPosition);

		if (USpawnComponent* SpawnComp = NewSign->FindComponentByClass<USpawnComponent>())
		{
			SpawnComp->ResetAsSpawned = false;
			SpawnComp->Spawn(false, true, false);
		}

		NewSign->InitialiseSign(signRequestData);

		success = true;
		return success;
	}

	return success;
}

void UGI_WarningSignSystem::ReturnSignToPool(AWarningSign* sign)
{
	if (!WarningSignPool.Active.Contains(sign))
	{
		return;
	}

	WarningSignPool.Active.Remove(sign);
	WarningSignPool.Inactive.Add(sign);
}



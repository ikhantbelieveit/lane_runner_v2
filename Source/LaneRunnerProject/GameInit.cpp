// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelSystem.h"
#include "ProjectileSystem.h"
#include "UIStateSystem.h"
#include "TestLevelUISystem.h"
#include "EUIState.h"
#include "GameInit.h"

// Sets default values
AGameInit::AGameInit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameInit::BeginPlay()
{
	Super::BeginPlay();
	
	if (CreateSystemsFromConfigData())
	{
		HasInitFinished = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERROR: failed system spawn on begin play."));
	}
	
}

// Called every frame
void AGameInit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasInitFinished)
	{
		if (!HasBroadcastInitFinished)
		{
			BroadcastInitFinished();

			AUIStateSystem* uiSystem = Cast<AUIStateSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIStateSystem::StaticClass()));
			if (uiSystem)
			{
				//show test ui
				uiSystem->EnterScreen(EUIState::TestLevel);
			}

			ALevelSystem* levelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
			if (levelSystem)
			{
				//show test ui
				levelSystem->SetScore(0);
			}

			HasBroadcastInitFinished = true;
		}
	}
}

void AGameInit::BroadcastInitFinished()
{
	if (HasBroadcastInitFinished)
	{
		return;
	}

	OnAllSystemsSpawned.Broadcast();

	HasBroadcastInitFinished = true;

}

bool AGameInit::CreateSystemsFromConfigData()
{
	bool success = false;

	if (ConfigData)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (const TPair <EGameSystem, FInitSystemData> pair : ConfigData->SystemConfigMap)
			{
				EGameSystem SystemType = pair.Key;
				const FInitSystemData& InitData = pair.Value;

				TSubclassOf<ABaseGameSystem> systemToSpawn = InitData.SystemClass;
				USystemConfigData* configDataAsset = InitData.ConfigDataAsset;

				if (systemToSpawn)
				{
					ABaseGameSystem* newSystem = World->SpawnActor<ABaseGameSystem>(systemToSpawn);
					if (newSystem)
					{
						if (configDataAsset)
						{
							newSystem->SetupFromConfig(configDataAsset);
						}

						SpawnedGameSystems.Add(newSystem);

						FString name = newSystem->GetName();
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, name);
					}
				}
			}


			success = true;
		}
	}

	return success;
}

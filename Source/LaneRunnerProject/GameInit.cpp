// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelSystem.h"
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
	
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<TSubclassOf<ABaseGameSystem>> systemsToSpawn = {
		ALevelSystem::StaticClass()
		};

		for (TSubclassOf<ABaseGameSystem> systemToSpawn : systemsToSpawn)
		{
			ABaseGameSystem* newSystem = World->SpawnActor<ABaseGameSystem>(systemToSpawn);

			if (newSystem)
			{
				SpawnedGameSystems.Add(newSystem);


				FString name = newSystem->GetName();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, name);
			}
		}
	}

	HasInitFinished = true;
}

// Called every frame
void AGameInit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasInitFinished)
	{
		BroadcastInitFinished();
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

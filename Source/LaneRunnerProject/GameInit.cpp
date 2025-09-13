// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelSystem.h"
#include "ProjectileSystem.h"
#include "UIStateSystem.h"
#include "TestLevelUISystem.h"
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
		ALevelSystem::StaticClass(),
		AProjectileSystem::StaticClass(),
		AUIStateSystem::StaticClass(),
		ATestLevelUISystem::StaticClass(),
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

	
	ShowTestUI();
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

			ALevelSystem* foundSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
			if (foundSystem)
			{
				//show test ui
				foundSystem->SetScore(0);
			}

			HasBroadcastInitFinished = true;
		}
	}
}

void AGameInit::ShowTestUI()
{
	AUIStateSystem* foundSystem = Cast<AUIStateSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIStateSystem::StaticClass()));
	if (foundSystem)
	{
		//show test ui
		foundSystem->EnterScreen();
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

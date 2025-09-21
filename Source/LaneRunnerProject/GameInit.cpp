// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_LevelSystem.h"
#include "ProjectileSystem.h"
#include "GI_UIStateSystem.h"
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
	
	
}

// Called every frame
void AGameInit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasBroadcastInitFinished)
	{
		auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
		if (uiStateSystem)
		{
			if (uiStateSystem->HasInitialisedFromConfig)
			{
				BroadcastInitFinished();

				auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
				if (levelSystem)
				{
					//show test ui
					levelSystem->EnterLevel();
				}

				
			}
		}

		//init audio system here


		HasBroadcastInitFinished = true;
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

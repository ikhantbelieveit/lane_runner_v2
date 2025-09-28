// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_LevelSystem.h"
#include "ProjectileSystem.h"
#include "GI_UIStateSystem.h"
#include "TestLevelUISystem.h"
#include "EUIState.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
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
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI)
		{
			if (GI->AllSystemsReady())
			{
				BroadcastInitFinished();

				AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

				if (playerActor)
				{
					APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
					if (playerRef)
					{
						playerRef->SetCharacterType(InitCharacterType);
					}
				}

				auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
				if (levelSystem)
				{
					levelSystem->EnterLevel();
					HasBroadcastInitFinished = true;
				}
			}
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

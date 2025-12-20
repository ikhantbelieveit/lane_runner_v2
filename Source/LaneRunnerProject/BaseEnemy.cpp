// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "LineOfSightComponent.h"
#include "DestructibleObjectComponent.h"
#include "GI_AudioSystem.h"
#include "GI_ProjectileSystem.h"
#include "GI_LevelSystem.h"
#include "SpawnComponent.h"
#include "TimedActionComponent.h"

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
	UPaperSpriteComponent* foundMainVisuals = nullptr;

	UPaperFlipbookComponent* foundMainVisualsFlipbook = nullptr;

	TArray<UPaperSpriteComponent*> spriteComps;
	TArray<UPaperFlipbookComponent*> flipbookComps;

	GetComponents<UPaperSpriteComponent>(spriteComps);
	GetComponents<UPaperFlipbookComponent>(flipbookComps);

	for (UPaperSpriteComponent* sprite : spriteComps)
	{
		if (!sprite)
		{
			continue;
		}

		if (sprite->ComponentHasTag("AlertSprite"))
		{
			foundAlertSprite = sprite;
		}

		if (sprite->ComponentHasTag("MainVisuals"))
		{
			foundMainVisuals = sprite;
		}
	}

	for (UPaperFlipbookComponent* flipbook : flipbookComps)
	{
		if (!flipbook)
		{
			continue;
		}

		if (flipbook->ComponentHasTag("MainVisuals"))
		{
			foundMainVisualsFlipbook = flipbook;
		}
	}

	if (foundAlertSprite)
	{
		AlertVFX = foundAlertSprite;
		AlertVFX->SetVisibility(false);
	}

	if (foundMainVisuals)
	{
		MainVisuals = foundMainVisuals;
		MainVisuals->SetSprite(IdleSprite);
	}

	if (foundMainVisualsFlipbook)
	{
		MainVisualsFlipbook = foundMainVisualsFlipbook;
		MainVisualsFlipbook->SetFlipbook(DefaultFlipbook);
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

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &ABaseEnemy::OnLevelReset);
	}

	TArray<USceneComponent*> sceneComponents;
	GetComponents<USceneComponent>(sceneComponents);

	for (USceneComponent* comp : sceneComponents)
	{
		if (!comp) continue;

		for (const FName& tag : comp->ComponentTags)
		{
			FString tagStr = tag.ToString();
			if (tagStr.StartsWith(TEXT("PROJ_")))
			{
				// Strip prefix to get clean key name
				FString originName = tagStr.Mid(5);
				ProjectileOrigins.Add(FName(*originName), comp);
				break;
			}
		}
	}

	TArray<UBoxComponent*> boxComps;
	GetComponents<UBoxComponent>(boxComps);

	for (UBoxComponent* box : boxComps)
	{
		if (box->ComponentHasTag("PhysicsBox"))
		{
			PhysicsBox = box;
			break;
		}
	}

	UTimedActionComponent* actionComponent = GetComponentByClass<UTimedActionComponent>();
	if (actionComponent)
	{
		actionComponent->PerformActionEvent.AddDynamic(this, &ABaseEnemy::PerformTimedAction);
		actionComponent->StartAction();
	}
}

bool ABaseEnemy::HasPerformedDetectAction()
{
	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::Shoot_OneOff:
		return PerformedOneOffShoot;
	case EEnemyDetectBehaviour::StraightAdvance:
		return PerformedAdvance;
	}
	return false;
}



// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemy::OnLevelReset()
{
	PerformedOneOffShoot = false;
	PerformedAdvance = false;

	if (AlertVFX)
	{
		AlertVFX->SetVisibility(false);
	}

	if (MainVisuals)
	{
		MainVisuals->SetSprite(IdleSprite);
	}

	if (MainVisualsFlipbook)
	{
		MainVisualsFlipbook->SetFlipbook(DefaultFlipbook);
	}
}

void ABaseEnemy::SetAnim(FString animName)
{
	UPaperFlipbook** foundAnim = FlipbookMap.Find(FName(animName));
	if (foundAnim)
	{
		MainVisualsFlipbook->SetFlipbook(*foundAnim);
	}
}

void ABaseEnemy::OnDetectPlayer()
{
	if (!IsAlive)
	{
		return;
	}

	if (HasPerformedDetectAction())
	{
		return;
	}

	if (DetectBehaviour == EEnemyDetectBehaviour::None)
	{
		return;
	}

	ULocationManagerComponent* locManager = GetComponentByClass<ULocationManagerComponent>();
	ULineOfSightComponent* lineOfSight = GetComponentByClass<ULineOfSightComponent>();

	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::StraightAdvance:
		if (locManager)
		{
			locManager->StartAutoMove(AdvanceDirection, AdvanceSpeed, false, FVector::Zero(), false);
			PerformedAdvance = true;
		}
		break;
	case EEnemyDetectBehaviour::Shoot_OneOff:
		auto* projSystem = GetGameInstance()->GetSubsystem<UGI_ProjectileSystem>();

		if (lineOfSight)
		{
			TArray<FName> shootProjNames = lineOfSight->GetSightZoneProjNames();

			for (FName projName : shootProjNames)
			{
				if (!TryPerformShoot(projName))
				{
					//complain here
				}
				else
				{
					PerformedOneOffShoot = true;
				}
			}

			//for (FName projName : shootProjNames)
			//{
			//	FProjectileRequestData* projRequest = ProjectileDataMap.Find(projName);
			//	for (FShootItem& item : projRequest->Items)
			//	{
			//		FVector shootPos = GetActorLocation();

			//		if (item.ShootOriginName != NAME_None)
			//		{
			//			if (USceneComponent** foundComp = ProjectileOrigins.Find(item.ShootOriginName))
			//			{
			//				if (*foundComp)
			//				{
			//					shootPos = (*foundComp)->GetComponentLocation();
			//				}
			//			}
			//		}

			//		item.ShootPos = shootPos;
			//	}

			//	if (!projSystem->ProcessProjectileRequest(*projRequest))
			//	{
			//		//complain here
			//	}
			//	else
			//	{
			//		PerformedOneOffShoot = true;
			//	}
			//}
		}

		
		
		break;
	}

	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
	audioSystem->Play(EAudioKey::EnemyAlert);

	if (AlertVFX && ShowAlertVFX)
	{
		AlertVFX->SetVisibility(true);
	}

	if (MainVisuals)
	{
		MainVisuals->SetSprite(AlertSprite);
	}

	FName setAnimName = NAME_None;

	if (lineOfSight->GetSetAnimName(setAnimName) && setAnimName != NAME_None)
	{
		SetAnim(setAnimName.ToString());
	}

	if (bScrollOnPlayerDetect)
	{
		if (locManager)
		{
			locManager->bScrollEnabled = true;
		}
	}
}

bool ABaseEnemy::IsEnemyAlive()
{
	return IsAlive;
}

void ABaseEnemy::OnKilled()
{
	IsAlive = false;

	if (AlertVFX)
	{
		AlertVFX->SetVisibility(false);
	}
}

void ABaseEnemy::OnAddedToGroup_Implementation(AActor* InGroupActor, ULocationManagerComponent* Manager)
{
	GroupActorRef = InGroupActor;
	GroupManagerRef = Manager;

	UE_LOG(LogTemp, Log, TEXT("%s added to group %s"), *GetName(), *InGroupActor->GetName());
}

void ABaseEnemy::OnRemovedFromGroup_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s removed from group"), *GetName());
	GroupActorRef = nullptr;
	GroupManagerRef = nullptr;
}

void ABaseEnemy::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{
	if (Entry.Metadata.IsEmpty()) return;

	// Parse JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bool bScrollEnabled = false;
		if (JsonObject->HasField(TEXT("ScrollEnabled")))
		{
			bScrollEnabled = JsonObject->GetBoolField(TEXT("ScrollEnabled"));

		}


		float ScrollXPos = 0.0f;
		if (JsonObject->HasField(TEXT("ScrollXPos")))
		{
			ScrollXPos = JsonObject->GetNumberField(TEXT("ScrollXPos"));
		}

		bool bStartSpawned = false;
		if (JsonObject->HasField(TEXT("StartSpawned")))
		{
			bStartSpawned = JsonObject->GetBoolField(TEXT("StartSpawned"));
		}

		bool bSetAdvanceDirection = false;
		EProjectileDirection newAdvanceDir = EProjectileDirection::None;

		if (JsonObject->HasField(TEXT("AdvanceDirection")))
		{
			bSetAdvanceDirection = true;

			// JSON numbers come back as double
			int32 RawValue = static_cast<int32>(JsonObject->GetNumberField(TEXT("AdvanceDirection")));
			newAdvanceDir = static_cast<EProjectileDirection>(RawValue);
		}

		if (bSetAdvanceDirection)
		{
			AdvanceDirection = newAdvanceDir;
		}


		if (ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>())
		{
			locManager->bStartScrollActive = bScrollEnabled;
			locManager->bScrollEnabled = bScrollEnabled;
			locManager->ScrollWithXPos = ScrollXPos;
		}

		if (USpawnComponent* spawnComp = FindComponentByClass<USpawnComponent>())
		{
			spawnComp->ResetAsSpawned = bStartSpawned;
		}
	}
}

void ABaseEnemy::PerformJump()
{
	if (!PhysicsBox)
	{
		return;
	}

	FVector jumpVector = FVector(0.0f, 0.0f, JumpVelocity);
	PhysicsBox->AddImpulse(jumpVector, NAME_None, true);
}

void ABaseEnemy::PerformNextTimedShoot()
{
	if (!TimedActionShootNames.IsValidIndex(TimedActionShootIndex))
	{
		return;
	}

	FName timedShootName = TimedActionShootNames[TimedActionShootIndex];

	if (!TryPerformShoot(timedShootName))
	{
		//complain here
	}

	TimedActionShootIndex = (TimedActionShootIndex + 1) % TimedActionShootNames.Num();
}

bool ABaseEnemy::TryPerformShoot(FName name)
{
	auto* projSystem = GetGameInstance()->GetSubsystem<UGI_ProjectileSystem>();
	if (!projSystem)
	{
		return false;
	}

	FProjectileRequestData* projRequest = ProjectileDataMap.Find(name);
	if (!projRequest)
	{
		UE_LOG(LogTemp, Warning, TEXT("No projectile data for %s"), *name.ToString());
		return false;
	}

	FProjectileRequestData LocalRequest = *projRequest;

	for (FShootItem& item : LocalRequest.Items)
	{
		FVector shootPos = GetActorLocation();

		if (item.ShootOriginName != NAME_None)
		{
			if (USceneComponent** foundComp = ProjectileOrigins.Find(item.ShootOriginName))
			{
				if (*foundComp)
				{
					shootPos = (*foundComp)->GetComponentLocation();

					GEngine->AddOnScreenDebugMessage(
						-1,
						2.0f,
						FColor::Green,
						FString::Printf(
							TEXT("Shoot from %s @ %.2f"),
							*item.ShootOriginName.ToString(),
							GetWorld()->TimeSeconds
						)
					);
				}
			}
		}

		item.ShootPos = shootPos;
	}

	return projSystem->ProcessProjectileRequest(LocalRequest);
}

void ABaseEnemy::PerformTimedAction()
{
	if (!IsAlive)
	{
		return;
	}

	switch (TimedActionType)
	{
	case EEnemyTimedActionType::Jump:
		PerformJump();
		break;
	case EEnemyTimedActionType::Shoot:
		PerformNextTimedShoot();
		break;
	}
}

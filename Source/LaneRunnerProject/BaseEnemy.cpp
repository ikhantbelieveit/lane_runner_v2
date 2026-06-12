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
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	
}

void ABaseEnemy::InitializeFromChunk_Implementation()
{
	ULineOfSightComponent* lineOfSight = GetComponentByClass<ULineOfSightComponent>();
	if (lineOfSight)
	{
		lineOfSight->OnDetectPlayer.AddDynamic(this, &ABaseEnemy::OnLineOfSightDetect);
	}

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

		if (sprite->ComponentHasTag("MainVisuals"))
		{
			MainVisuals = sprite;
			break;
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
			MainVisualsFlipbook = flipbook;
			break;
		}
	}

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
			destructible->OnDestroyed.AddDynamic(this, &ABaseEnemy::OnKilled);
			break;
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
		actionComponent->StartDelay = TimedActionOffset;
	}

	TArray<USceneComponent*> sceneComponents;
	GetComponents<USceneComponent>(sceneComponents);

	ProjectileOrigins.Empty();
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
}

void ABaseEnemy::ResetOnChunkRequest_Implementation()
{
	PerformedOneOffShoot = false;
	PerformedAdvance = false;
	IsAlive = true;
	IsAlert = false;

	if (MainVisuals)
	{
		MainVisuals->SetSprite(IdleSprite);
	}
	
	if (MainVisualsFlipbook)
	{
		MainVisualsFlipbook->SetFlipbook(DefaultFlipbook);
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
	case EEnemyDetectBehaviour::StartTimedAction:
		return false;
	}
	return false;
}

void ABaseEnemy::SetAnim(FString animName)
{
	UPaperFlipbook** foundAnim = FlipbookMap.Find(FName(animName));
	if (foundAnim)
	{
		MainVisualsFlipbook->SetFlipbook(*foundAnim);
	}
}

void ABaseEnemy::OnLineOfSightDetect()
{
	OnDetectPlayer(true);
}

void ABaseEnemy::OnDetectPlayer(bool bPlayAlertSound)
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

	USpawnComponent* spawnComp = GetComponentByClass<USpawnComponent>();
	if (!spawnComp->GetCurrentSpawned())
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
		}

		break;
	case EEnemyDetectBehaviour::StartTimedAction:
	
		auto* timedAction = GetComponentByClass<UTimedActionComponent>();
		if (timedAction)
		{
			timedAction->StartAction();
		}
		break;
	
	}

	if (bPlayAlertSound)
	{
		auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
		audioSystem->Play(EAudioKey::EnemyAlert);
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

	IsAlert = true;
}

bool ABaseEnemy::IsEnemyAlive()
{
	return IsAlive;
}

void ABaseEnemy::OnKilled()
{
	IsAlive = false;
	OnEnemyKilled.Broadcast();
}

void ABaseEnemy::OnAddedToGroup_Implementation(AActor* InGroupActor, ULocationManagerComponent* Manager)
{
	GroupActorRef = InGroupActor;
	GroupManagerRef = Manager;

	GroupManagerRef->OnMoveToDespawnStart.AddDynamic(this, &ABaseEnemy::OnMoveToDespawnStart);
	GroupManagerRef->OnAutoMoveEnd.AddDynamic(this, &ABaseEnemy::OnAutoMoveEnd);

	UE_LOG(LogTemp, Log, TEXT("%s added to group %s"), *GetName(), *InGroupActor->GetName());
}

void ABaseEnemy::OnRemovedFromGroup_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s removed from group"), *GetName());
	GroupActorRef = nullptr;
	GroupManagerRef = nullptr;
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

void ABaseEnemy::OnAutoMoveEnd()
{
	if (SetAlertOnAutoMoveEnd && !IsAlert)
	{
		OnDetectPlayer(false);
		SetAnim("Alert");
	}
}

void ABaseEnemy::OnMoveToDespawnStart()
{
	if (SetIdleOnMoveToDespawn && IsAlert)
	{
		SetIdle();
	}
}

void ABaseEnemy::SetIdle()
{
	if (!IsAlive)
	{
		return;
	}

	IsAlert = false;

	if (MainVisualsFlipbook)
	{
		MainVisualsFlipbook->SetFlipbook(DefaultFlipbook);
	}

	switch (DetectBehaviour)
	{
	case EEnemyDetectBehaviour::StartTimedAction:
		auto* timedAction = GetComponentByClass<UTimedActionComponent>();
		if (timedAction)
		{
			timedAction->StopAction();
		}
		break;
	}

	//TODO: deactivate specific Alert Behaviours as needed 
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelSystem.h"
#include "GI_UIStateSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GI_AudioSystem.h"
#include "GI_SaveSystem.h"
#include "LocationManagerComponent.h"
#include "Components/SplineComponent.h"
#include "PlayerDetectComponent.h"
#include "SpawnComponent.h"
#include "GI_ChunkManagerSystem.h"
#include "GI_LevelGenerationSystem.h"
#include "GameInit.h"
#include "BaseEnemy.h"
#include "BaseCollectible.h"
#include "GroupOwnerComponent.h"

void UGI_LevelSystem::OnGameOverDelayComplete()
{
	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	//auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();

	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::DeathScreen);
	}
}

void UGI_LevelSystem::SetGameState(EGameState newState)
{
	if (CurrentGameState == newState)
	{
		return;
	}

	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
    auto* saveSystem = GetGameInstance()->GetSubsystem<UGI_SaveSystem>();

	switch (newState)
	{
    case EGameState::Paused:
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        OnPause.Broadcast();
        break;
	case EGameState::Lose:

        CalculateScoreResult(ScoreResult);
        
        if (saveSystem)
        {
            if (saveSystem->HasExistingSave())
            {
                PrevSaveStatsCache = saveSystem->CurrentSave->StatsData;
            }
        }

        OnPlayerLose.Broadcast();

		SaveLevelStats();

		if (audioSystem)
		{
			audioSystem->StopMusic();
		}

		GetWorld()->GetTimerManager().SetTimer(
			GameOverDelayHandle,
			this,
			&UGI_LevelSystem::OnGameOverDelayComplete,
			GameOverDelay,
			false,
			-1.0f
		);

		break;
	case EGameState::Active:

		if (uiStateSystem)
		{
			uiStateSystem->EnterScreen(EUIState::TestLevel);
		}

		if (audioSystem)
		{
			audioSystem->PlayMusic(EAudioKey::BackgroundMusic);
		}

        if (CurrentGameState == EGameState::Paused)
        {
            UGameplayStatics::SetGamePaused(GetWorld(), false);
            OnUnpause.Broadcast();
        }
        
		break;
	}

	PrevGameState = CurrentGameState;
	CurrentGameState = newState;

	OnGameStateChanged.Broadcast(CurrentGameState, PrevGameState);
}

EGameState UGI_LevelSystem::GetGameState()
{
	return CurrentGameState;
}

void UGI_LevelSystem::OnPlayerTouchHazard(bool oneHitKill, bool overrideInvincibility)
{
	if (GetGameState() == EGameState::Active)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
		if (player)
		{
			if (player->GetMercyInvincibleActive() && !overrideInvincibility)
			{
				return;
			}

			player->OnHitHazard(oneHitKill);
			if (player->GetCurrentHealth() <= 0)
			{
				SetGameState(EGameState::Lose);
			}
		}
	}
}

void UGI_LevelSystem::ResetLevelStats()
{
	SetScore(0);
	PointsUntilNextThreshold = PointsHealThreshold;
}

void UGI_LevelSystem::ResetFromLose()
{

	ResetLevelStats();
    RegenerateLevel();

    CleanupBeforeReset.Broadcast();

	SetGameState(EGameState::Active);
}

void UGI_LevelSystem::RestartLevel()
{
    OnLevelRestart.Broadcast();
    ResetLevelStats();
    RegenerateLevel();

    CleanupBeforeReset.Broadcast();

    SetGameState(EGameState::Active);
}

void UGI_LevelSystem::KillPlayer()
{
	SetGameState(EGameState::Lose);
}

void UGI_LevelSystem::OnPitfall()
{
	if (GetGameState() == EGameState::Active)
	{
		KillPlayer();

		auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
		if (audioSystem)
		{
			audioSystem->Play(EAudioKey::Pitfall);
		}
	}
}

void UGI_LevelSystem::SetScore(int newScore)
{
	CurrentScore = newScore;
	OnScoreSet.Broadcast();
}

void UGI_LevelSystem::AddToScore(int addValue)
{
	SetScore(CurrentScore + addValue);

	PointsUntilNextThreshold -= addValue;

	if (PointsUntilNextThreshold <= 0)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
		if (player)
		{
			player->TryAddHealth(1);
            auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
            if (audioSystem)
            {
                audioSystem->Play(EAudioKey::HealPlayer);
            }
		}

		PointsUntilNextThreshold = PointsHealThreshold + PointsUntilNextThreshold;
	}
}

int UGI_LevelSystem::GetScore()
{
	return CurrentScore;
}

void UGI_LevelSystem::EnterLevel()
{
    

	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->ResetPlayer();
	}

    RegenerateLevel();
    CleanupBeforeReset.Broadcast();
    UE_LOG(LogTemp, Warning, TEXT("[POOL] CleanupBeforeReset BROADCAST"));

	ResetLevelStats();

	SetGameState(EGameState::Active);
}

void UGI_LevelSystem::ExitLevel()
{
    CleanupBeforeReset.Broadcast();

    ResetLevelStats();

    SetGameState(EGameState::Dormant);

    OnLevelExit.Broadcast();
}

void UGI_LevelSystem::RegenerateLevel()
{
    auto* chunkSystem = GetGameInstance()->GetSubsystem<UGI_ChunkManagerSystem>();
    chunkSystem->ClearChunks();


    FLevelLayoutData levelLayoutData;

    if (auto* gameInit = Cast<AGameInit>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameInit::StaticClass())))
    {
        switch (gameInit->InitType)
        {
        case EInitLevelType::None:
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[LEVEL] Level Init type set to None - returning!"));
            return;
        case EInitLevelType::UsePremadeLevel:
        {
            levelLayoutData = gameInit->PremadeLevelAsset->Layout;
            auto* generationSystem = GetGameInstance()->GetSubsystem<UGI_LevelGenerationSystem>();
            int seed = gameInit->LevelGenSettingsAsset->GetSeed();
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("[LEVEL] Applying random elements to premade level with seed: %d"), seed));
            UE_LOG(LogTemp, Log, TEXT("[LEVEL] Randomizing level with seed: %d"), seed);
            FRandomStream random(seed);
            generationSystem->ResolveMissingChunkVariants(random, levelLayoutData);
        }
            
            break;
        case EInitLevelType::GenerateFromSettings:
            auto* generationSystem = GetGameInstance()->GetSubsystem<UGI_LevelGenerationSystem>();
            int seed = gameInit->LevelGenSettingsAsset->GetSeed();
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("[LEVEL] Generating level with seed: %d"), seed));
            UE_LOG(LogTemp, Log, TEXT("[LEVEL] Generating level with seed: %d"), seed);
            FRandomStream random(seed);
            if (!generationSystem->GenerateLevelLayout(gameInit->LevelGenSettingsAsset->Definition, random, levelLayoutData))
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[LEVEL] failed to generate level - returning!"));
                return;
            }
        }

    }

    chunkSystem->SpawnChunksFromLayoutData(levelLayoutData);
}

void UGI_LevelSystem::TriggerContinue()
{
	if (GetGameState() == EGameState::AwaitContinue)
	{
		SetGameState(EGameState::Active);
	}
}

void UGI_LevelSystem::SaveLevelStats()
{
	int highScore = 0;
    int highestDistance = 0;

    int oldHighScore = 0;
    int oldHighestDistance = 0;

    int currentScore = ScoreResult.TotalScore;
    int currentDistance = ScoreResult.DistanceBonus.BonusValue;

	auto* saveSystem = GetGameInstance()->GetSubsystem<UGI_SaveSystem>();
	if (saveSystem)
	{
		if (saveSystem->HasExistingSave())
		{
            oldHighScore = saveSystem->CurrentSave->StatsData.HighScore;
            oldHighestDistance = saveSystem->CurrentSave->StatsData.HighestDistance;
		}
	}

    highScore = (currentScore > oldHighScore) ? currentScore : oldHighScore;
    highestDistance = (currentDistance > oldHighestDistance) ? currentDistance : oldHighestDistance;

	if (saveSystem)
	{
		UMySaveGame* saveObject = Cast<UMySaveGame>(
			UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

		saveObject->StatsData.HighScore = highScore;
        saveObject->StatsData.HighestDistance = highestDistance;
		saveSystem->SaveGame(saveObject);
	};
}

void UGI_LevelSystem::ExecuteEvents(const TArray<FLevelEventData>& Events)
{
    
	for (const FLevelEventData& Event : Events)
	{
		ExecuteSingleEvent(Event);
	}
}

void UGI_LevelSystem::TogglePause()
{
    switch (CurrentGameState)
    {
    case EGameState::Active:
        SetGameState(EGameState::Paused);
        break;
    case EGameState::Paused:
        SetGameState(EGameState::Active);
        break;
    }
}

void UGI_LevelSystem::ExecuteSingleEvent(const FLevelEventData& Event)
{
    if (!IsInGameThread())
    {
        UE_LOG(LogTemp, Warning, TEXT("ExecuteSingleEvent called off game thread. Scheduling on game thread."));
        AsyncTask(ENamedThreads::GameThread, [this, Event]() { ExecuteSingleEvent(Event); });
        return;
    }

    switch (Event.EventType)
    {
    case ELevelEventType::TogglePlayerScroll:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target))
            {
                continue;
            }

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->bScrollEnabled = Event.BoolParam;
            }
        }
        break;
    }

    case ELevelEventType::SetObjectPath:
    {
        AActor* SplineOwner = Event.ActorParam.Get();
        if (!IsValid(SplineOwner))
        {
            UE_LOG(LogTemp, Warning, TEXT("SetObjectPath: ActorParam is invalid, skipping event."));
            break;
        }

        USplineComponent* SetSpline = SplineOwner->FindComponentByClass<USplineComponent>();
        if (!IsValid(SetSpline))
        {
            UE_LOG(LogTemp, Warning, TEXT("SetObjectPath: ActorParam has no valid SplineComponent, skipping event."));
            break;
        }

        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->SetSpline(SetSpline);
            }
        }
        break;
    }

    case ELevelEventType::TogglePathFollow:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->bFollowEnabled = Event.BoolParam;
            }
        }
        break;
    }

    case ELevelEventType::SetObjectSpeed:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->SetPathSpeed(Event.NumericParam);
            }
        }
        break;
    }

    case ELevelEventType::InvertSpeed:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->SetPathSpeed(-LocationComp->GetCurrentSpeed());
            }
        }
        break;
    }

    case ELevelEventType::SetAutoMoveSpeed:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->SetAutoMoveSpeed(Event.NumericParam);
            }
        }
        break;
    }

    case ELevelEventType::SetAutoMoveDirection:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->SetAutoMoveDirection(Event.DirectionParam);
            }
        }
        break;
    }

    case ELevelEventType::StartAutoMove:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->StartAutoMove(Event.DirectionParam, Event.NumericParam, Event.BoolParam, Event.VectorParam, false);
            }
        }
        break;
    }

    case ELevelEventType::MoveToDespawn:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->StartAutoMove(Event.DirectionParam, Event.NumericParam, Event.BoolParam, Event.VectorParam, true);
            }
        }
        break;
    }

    case ELevelEventType::MoveToAlert:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                if (UGroupOwnerComponent* GroupComp =
                    Target->FindComponentByClass<UGroupOwnerComponent>())
                {
                    // Prevent duplicate bindings
                    LocationComp->OnAutoMoveEnd.RemoveDynamic(
                        GroupComp,
                        &UGroupOwnerComponent::AlertAllGroupMembers
                    );

                    LocationComp->OnAutoMoveEnd.AddDynamic(
                        GroupComp,
                        &UGroupOwnerComponent::AlertAllGroupMembers
                    );
                }

                LocationComp->StartAutoMove(Event.DirectionParam, Event.NumericParam, Event.BoolParam, Event.VectorParam, false);
            }
        }
        break;
    }

    case ELevelEventType::StartAutoMoveGroup:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            TArray<AActor*> TargetChildren;
            Target->GetAllChildActors(TargetChildren, true);

            for (auto* child : TargetChildren)
            {
                if (ULocationManagerComponent* LocationComp = child->FindComponentByClass<ULocationManagerComponent>())
                {
                    LocationComp->StartAutoMove(Event.DirectionParam, Event.NumericParam, Event.BoolParam, Event.VectorParam, false);
                }
                else
                {

                }
            }
        }
        break;
    }

    case ELevelEventType::StopAutoMove:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (ULocationManagerComponent* LocationComp = Target->FindComponentByClass<ULocationManagerComponent>())
            {
                LocationComp->StopAutoMove(false);
            }
        }
        break;
    }

    case ELevelEventType::StopAutoMoveGroup:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            TArray<AActor*> TargetChildren;
            Target->GetAllChildActors(TargetChildren, true);

            for (auto* child : TargetChildren)
            {
                if (ULocationManagerComponent* LocationComp = child->FindComponentByClass<ULocationManagerComponent>())
                {
                    LocationComp->StopAutoMove(false);
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("NO LOCATION COMPONENT FOUND"));
                }
            }
        }
        break;
    }

    case ELevelEventType::SpawnObjects:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (USpawnComponent* SpawnComp = Target->FindComponentByClass<USpawnComponent>())
            {
                const bool Drop = false;
                const bool ScrollInstant = Event.BoolParam;
                const bool ScrollOnReach = false;
                SpawnComp->Spawn(Drop, ScrollInstant, ScrollOnReach);
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("NO SPAWN COMPONENT FOUND"));
            }
        }
        break;
    }

    case ELevelEventType::SpawnObjectGroup:
    {
        const bool Drop = false;
        const bool ScrollInstant = Event.BoolParam;
        const bool ScrollOnReach = false;

        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            TArray<AActor*> TargetChildren;
            Target->GetAllChildActors(TargetChildren, true);

            for (auto* child : TargetChildren)
            {
                if (USpawnComponent* SpawnComp = child->FindComponentByClass<USpawnComponent>())
                {
                    
                    SpawnComp->Spawn(Drop, false, false);
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("NO SPAWN COMPONENT FOUND"));
                }
            }

            //this is where we actually set scroll 
            if (ULocationManagerComponent* groupLocManager = Target->GetComponentByClass<ULocationManagerComponent>())
            {
                groupLocManager->bScrollEnabled = ScrollInstant;
            }

            if (ScrollOnReach)
            {
                UPlayerDetectComponent* detectComp = Cast<UPlayerDetectComponent>(Target->GetComponentByClass(UPlayerDetectComponent::StaticClass()));
                if (detectComp)
                {
                    FLevelEventData NewEvent;
                    NewEvent.EventType = ELevelEventType::TogglePlayerScroll;

                    NewEvent.TargetActors.Add(Target);

                    NewEvent.BoolParam = true;
                    detectComp->EventsToTrigger.Empty();
                    detectComp->EventsToTrigger.Add(NewEvent);
                }
            }
        }
        break;
    }

    case ELevelEventType::DespawnObjects:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (USpawnComponent* SpawnComp = Target->FindComponentByClass<USpawnComponent>())
            {
                SpawnComp->Despawn();
            }
        }
        break;
    }

    case ELevelEventType::DespawnObjectGroup:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            TArray<AActor*> TargetChildren;
            Target->GetAllChildActors(TargetChildren, true);

            for (auto* child : TargetChildren)
            {
                if (USpawnComponent* SpawnComp = child->FindComponentByClass<USpawnComponent>())
                {
                    SpawnComp->Despawn();
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("NO SPAWN COMPONENT FOUND"));
                }
            }
        }
        break;
    }

    case ELevelEventType::SetAlert:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            TArray<AActor*> TargetChildren;
            Target->GetAllChildActors(TargetChildren, true);

            for (auto* child : TargetChildren)
            {
                if (ABaseEnemy* enemy = Cast<ABaseEnemy>(child))
                {
                    enemy->OnDetectPlayer(true);
                    enemy->SetAnim("Alert");
                }
            }
        }
        break;
    }

    case ELevelEventType::SetIdle:
    {
        for (TObjectPtr<AActor> TargetPtr : Event.TargetActors)
        {
            AActor* Target = TargetPtr.Get();
            if (!IsValid(Target)) continue;

            if (UGroupOwnerComponent* GroupComp =
                Target->FindComponentByClass<UGroupOwnerComponent>())
            {
                GroupComp->UnAlertAllGroupMembers();
            }
        }
        
        break;
    }

    default:
        break;
    }
}

void UGI_LevelSystem::CalculateScoreResult(FLevelScoreResult& result)
{
    result.BasePoints = GetScore();

    APlayerCharacter* playerRef = nullptr;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        AActor* foundActor = FoundActors[0];
        playerRef = Cast<APlayerCharacter>(foundActor);
    }

    result.DistanceBonus = FLevelScoreBonus();

    if (playerRef)
    {
        result.DistanceBonus.BonusValue = playerRef->GetDistanceTravelled_Meters();
    }

    result.DistanceBonus.Multiplier = 0.45f;
    result.DistanceBonus.TotalBonus = result.DistanceBonus.BonusValue * result.DistanceBonus.Multiplier;

    result.TotalScore = result.BasePoints + result.DistanceBonus.TotalBonus;
}

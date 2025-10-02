// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelSystem.h"
#include "GI_UIStateSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GI_AudioSystem.h"
#include "GI_SaveSystem.h"
#include "LocationManagerComponent.h"
#include "Components/SplineComponent.h"
#include "SpawnComponent.h"

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
	case EGameState::Lose:

        
        if (saveSystem)
        {
            if (saveSystem->HasExistingSave())
            {
                HighScoreAtTimeOfDeath = saveSystem->CurrentSave->StatsData.HighScore;
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

void UGI_LevelSystem::ResetLevel()
{
	SetScore(0);
	PointsUntilNextThreshold = PointsHealThreshold;
}

void UGI_LevelSystem::ResetFromLose()
{
	CleanupBeforeReset.Broadcast();
	ResetLevel();

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

	CleanupBeforeReset.Broadcast();

	ResetLevel();

	SetGameState(EGameState::Active);
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

	auto* saveSystem = GetGameInstance()->GetSubsystem<UGI_SaveSystem>();
	if (saveSystem)
	{
		if (saveSystem->HasExistingSave())
		{
			highScore = saveSystem->CurrentSave->StatsData.HighScore;
		}
	}

	if (GetScore() > highScore)
	{
		highScore = GetScore();
	}
	if (saveSystem)
	{
		UMySaveGame* saveObject = Cast<UMySaveGame>(
			UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

		saveObject->StatsData.HighScore = highScore;
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
                LocationComp->StartAutoMove(Event.DirectionParam, Event.NumericParam, Event.BoolParam, Event.VectorParam);
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

    default:
        break;
    }
}
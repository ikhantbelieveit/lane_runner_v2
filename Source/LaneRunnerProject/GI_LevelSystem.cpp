// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_LevelSystem.h"
#include "GI_UIStateSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GI_AudioSystem.h"
#include "GI_SaveSystem.h"
#include "LocationManagerComponent.h"
#include "Components/SplineComponent.h"

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

	switch (newState)
	{
	case EGameState::Lose:
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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("broadcast CleanupBeforeReset"));
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

	if (PointsUntilNextThreshold <= 1)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
		if (player)
		{
			player->TryAddHealth(1);
		}

		PointsUntilNextThreshold = PointsHealThreshold;
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

		// Fill in some data
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
	switch (Event.EventType)
	{
	case ELevelEventType::TogglePlayerScroll:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->bScrollEnabled = Event.BoolParam;
				}
			}
		}
		break;
	}

	case ELevelEventType::SetObjectPath:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					if (USplineComponent* setSpline = Event.ActorParam->FindComponentByClass<USplineComponent>())
					{
						locationComp->SetSpline(setSpline);
					}
				}
			}
		}
		break;
	}

	case ELevelEventType::TogglePathFollow:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->bFollowEnabled = Event.BoolParam;
				}
			}
		}
		break;
	}

	case ELevelEventType::SetObjectSpeed:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->SetPathSpeed(Event.NumericParam);
				}
			}
		}
		break;
	}

	case ELevelEventType::InvertSpeed:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->SetPathSpeed(-locationComp->GetCurrentSpeed());
				}
			}
		}
		break;
	}

	case ELevelEventType::SetAutoMoveSpeed:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					if (USplineComponent* setSpline = Target->FindComponentByClass<USplineComponent>())
					{
						locationComp->SetAutoMoveSpeed(Event.NumericParam);
					}
				}
			}
		}
		break;
	}

	case ELevelEventType::SetAutoMoveDirection:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->SetAutoMoveDirection(Event.DirectionParam);
				}
			}
		}
		break;
	}

	case ELevelEventType::StartAutoMove:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->bAutoMoveEnabled = true;
					locationComp->ApplyAutoMove();
				}
			}
		}
		break;
	}

	case ELevelEventType::StopAutoMove:
	{
		for (AActor* Target : Event.TargetActors)
		{
			if (Target)
			{
				if (ULocationManagerComponent* locationComp = Target->FindComponentByClass<ULocationManagerComponent>())
				{
					locationComp->StopAutoMove();
				}
			}
		}
		break;
	}

	default:
	{
		// For Blueprint implementable extension
		break;
	}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUIWidget.h"
#include "GI_LevelSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void ULevelUIWidget::Initialise()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		AActor* foundActor = FoundActors[0];
		APlayerCharacter* player = Cast<APlayerCharacter>(foundActor);
		player->OnHealthSet.AddDynamic(this, &ULevelUIWidget::OnHealthUpdate);
	}

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->OnScoreSet.AddDynamic(this, &ULevelUIWidget::OnScoreUpdate);
	}
}

void ULevelUIWidget::OnScoreUpdate()
{
	int score = 0;
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		score = levelSystem->GetScore();
	}


	if (ScoreText)
	{
		FString scoreString = FString::FromInt(score);
		FText text = FText::FromString(scoreString);
		ScoreText->SetText(text);
	}
}

void ULevelUIWidget::OnHealthUpdate()
{
	int health = 0;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		AActor* foundActor = FoundActors[0];
		APlayerCharacter* player = Cast<APlayerCharacter>(foundActor);
		health = player->CurrentHealth;
	}

	if (HealthText)
	{
		FString healthString = FString::FromInt(health);
		FText text = FText::FromString(healthString);
		HealthText->SetText(text);
	}
}

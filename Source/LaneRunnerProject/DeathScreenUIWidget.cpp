// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathScreenUIWidget.h"
#include "LevelSystem.h"
#include "Kismet/GameplayStatics.h"

void UDeathScreenUIWidget::Initialise()
{
	if (RespawnButton)
	{
		DefaultSelection = RespawnButton;
		RespawnButton->OnClicked.AddDynamic(this, &UDeathScreenUIWidget::OnRespawnButtonPressed);
	}
}

void UDeathScreenUIWidget::OnRespawnButtonPressed()
{
	ALevelSystem* foundLevelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
	if (foundLevelSystem)
	{
		foundLevelSystem->ResetFromLose();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "EventTrigger.h"
#include "GI_LevelSystem.h"
#include "EProjectileDirection.h"
#include "PlayerDetectComponent.h"

// Sets default values
AEventTrigger::AEventTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEventTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEventTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEventTrigger::InitializeFromChunk_Implementation()
{
	EventsToTrigger.Empty();

	for (const FEventTriggerData setupData : EventTriggerSetupData)
	{
		FLevelEventData eventData;
		eventData.EventType = setupData.EventType;
		eventData.TargetActorIDs = setupData.TargetActorIDs;
		EventsToTrigger.Add(eventData);
	}
}


void AEventTrigger::ResolveTargetActorIDs(ALevelChunkActor* parentChunk)
{
	for (FLevelEventData& Event : EventsToTrigger)
	{
		Event.TargetActors.Empty();
		for (const FName& ID : Event.TargetActorIDs)
		{
			bool success = false;
			AActor* foundActor = parentChunk->GetChildActorByID(ID, success);

			if (success && foundActor)
			{
				Event.TargetActors.Add(foundActor);
				/*GEngine->AddOnScreenDebugMessage(
					-1,
					4.f,
					FColor::Green,
					FString::Printf(TEXT("[ResolveTargetActorIDs] '%s' matched tag '%s' in chunk '%s'"),
						*foundActor->GetName(),
						*ID.ToString(),
						*parentChunk->GetName())
				);*/
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					4.f,
					FColor::Red,
					FString::Printf(TEXT("[ResolveTargetActorIDs] No match found for tag '%s' in chunk '%s'"),
						*ID.ToString(),
						*parentChunk->GetName())
				);
			}
		}
	}
}

void AEventTrigger::TriggerEvents()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, TEXT("TRIGGER EVENT"));
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
		{
			levelSystem->ExecuteEvents(EventsToTrigger);
		}
	}
}

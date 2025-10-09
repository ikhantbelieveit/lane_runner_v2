// Fill out your copyright notice in the Description page of Project Settings.


#include "EventTrigger.h"
#include "GI_LevelSystem.h"
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

void AEventTrigger::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{
	if (Entry.Metadata.IsEmpty()) return;

	EventsToTrigger.Empty();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* EventsJson;
		if (JsonObject->TryGetArrayField(TEXT("Events"), EventsJson))
		{
			for (const TSharedPtr<FJsonValue>& EventValue : *EventsJson)
			{
				TSharedPtr<FJsonObject> EventObj = EventValue->AsObject();
				if (!EventObj.IsValid()) continue;

				FLevelEventData EventData;

				// EventType
				int eventTypeInt;
				if (EventObj->TryGetNumberField(TEXT("EventType"), eventTypeInt))
				{
					EventData.EventType = static_cast<ELevelEventType>(eventTypeInt);
				}

				// TargetActorIDs
				const TArray<TSharedPtr<FJsonValue>>* TargetIDs;
				if (EventObj->TryGetArrayField(TEXT("TargetActorIDs"), TargetIDs))
				{
					for (const TSharedPtr<FJsonValue>& IDVal : *TargetIDs)
					{
						EventData.TargetActorIDs.Add(FName(*IDVal->AsString()));
					}
				}

				

				// NumericParam
				EventObj->TryGetNumberField(TEXT("NumericParam"), EventData.NumericParam);

				// BoolParam
				EventObj->TryGetBoolField(TEXT("BoolParam"), EventData.BoolParam);

				EventsToTrigger.Add(EventData);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("added awesome level event"));
			}
		}

		FString detectCollName = "PlayerColl";
		if (JsonObject->HasField(TEXT("DetectCollTag")))
		{
			detectCollName = JsonObject->GetStringField(TEXT("DetectCollTag"));
		}

		if (UPlayerDetectComponent* detectComp = FindComponentByClass<UPlayerDetectComponent>())
		{
			detectComp->PlayerAreaTag = FName(detectCollName);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set detect coll to"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, detectComp->PlayerAreaTag.ToString());
		}
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
				GEngine->AddOnScreenDebugMessage(
					-1,
					4.f,
					FColor::Green,
					FString::Printf(TEXT("[ResolveTargetActorIDs] '%s' matched tag '%s' in chunk '%s'"),
						*foundActor->GetName(),
						*ID.ToString(),
						*parentChunk->GetName())
				);
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
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
		{
			levelSystem->ExecuteEvents(EventsToTrigger);
		}
	}
}

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

void AEventTrigger::InitialiseFromChunk_Implementation()
{
	EventsToTrigger.Empty();

	for (const FEventTriggerData setupData : EventTriggerSetupData)
	{
		FLevelEventData eventData;
		eventData.EventType = setupData.EventType;
		eventData.TargetActorIDs = setupData.TargetActorIDs;
		EventsToTrigger.Add(eventData);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Magenta, TEXT("Add event data"));
	}
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

				// DirectionParam
				int directionParamInt;
				if (EventObj->TryGetNumberField(TEXT("DirectionParam"), directionParamInt))
				{
					EventData.DirectionParam = static_cast<EProjectileDirection>(directionParamInt);
				}

				// NumericParam
				float numParam;
				if (EventObj->TryGetNumberField(TEXT("NumericParam"), numParam))
				{
					EventData.NumericParam = numParam;
				}

				// BoolParam
				bool boolParam;
				if (EventObj->TryGetBoolField(TEXT("BoolParam"), boolParam))
				{
					EventData.BoolParam = boolParam;
				}

				if (EventObj->HasTypedField<EJson::Object>(TEXT("VectorParam")))
				{
					TSharedPtr<FJsonObject> VecObj = EventObj->GetObjectField(TEXT("VectorParam"));

					FVector ParsedVector;
					ParsedVector.X = VecObj->GetNumberField(TEXT("X"));
					ParsedVector.Y = VecObj->GetNumberField(TEXT("Y"));
					ParsedVector.Z = VecObj->GetNumberField(TEXT("Z"));

					// Assign it to your struct or component
					EventData.VectorParam = ParsedVector;

					UE_LOG(LogTemp, Warning, TEXT("Parsed VectorParam: X=%f Y=%f Z=%f"),
						ParsedVector.X, ParsedVector.Y, ParsedVector.Z);
				}
				
				EventsToTrigger.Add(EventData);
			}
		}

		FString detectCollName = "PlayerColl";
		if (JsonObject->HasField(TEXT("DetectCollTag")))
		{
			detectCollName = JsonObject->GetStringField(TEXT("DetectCollTag"));
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
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
		{
			levelSystem->ExecuteEvents(EventsToTrigger);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerDetectComponent.h"
#include "ScrollWithPlayerComponent.h"
#include "EventTrigger.h"

// Sets default values for this component's properties
UPlayerDetectComponent::UPlayerDetectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerDetectComponent::BeginPlay()
{
	Super::BeginPlay();



	if (!BoxComponent)
	{
		BoxComponent = GetOwner()->FindComponentByClass<UBoxComponent>();
		
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UPlayerDetectComponent::HandleBeginOverlap);
		
	}
}


// Called every frame
void UPlayerDetectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerDetectComponent::ResolveTargetActorIDs(ALevelChunkActor* parentChunk)
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

void UPlayerDetectComponent::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherComp->ComponentHasTag(PlayerAreaTag))
	{
		

		if (UseParentEventLogic)
		{
			if (AEventTrigger* trigger = Cast<AEventTrigger>(GetOwner()))
			{
				trigger->TriggerEvents();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("BOOM BABY"));
			}
		}
		else
		{
			if (UGameInstance* GI = GetWorld()->GetGameInstance())
			{
				if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
				{
					levelSystem->ExecuteEvents(EventsToTrigger);
				}
			}
		}
		
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerDetectComponent.h"
#include "ScrollWithPlayerComponent.h"

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
		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>())
			{
				levelSystem->ExecuteEvents(EventsToTrigger);
			}
		}
	}
}

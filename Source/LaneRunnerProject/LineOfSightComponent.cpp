// Fill out your copyright notice in the Description page of Project Settings.


#include "LineOfSightComponent.h"
#include "Components/BoxComponent.h"


// Sets default values for this component's properties
ULineOfSightComponent::ULineOfSightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULineOfSightComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UBoxComponent* foundLineOfSight = nullptr;

	TArray<UBoxComponent*> boxComps;

	GetOwner()->GetComponents<UBoxComponent>(boxComps);

	for (UBoxComponent* box : boxComps)
	{
		if (!box)
		{
			continue;
		}

		if (box->ComponentHasTag("LineOfSight"))
		{
			foundLineOfSight = box;
			break;
		}
	}
	

	if (foundLineOfSight)
	{
		foundLineOfSight->OnComponentBeginOverlap.AddDynamic(this, &ULineOfSightComponent::HandleBeginOverlap);
	}
}


void ULineOfSightComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp,
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

	if (OtherComp->ComponentHasTag("PlayerColl"))
	{
		OnDetect();
	}
}

void ULineOfSightComponent::OnDetect()
{
	OnDetectPlayer.Broadcast();

	
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactToProjComponent.h"

// Sets default values for this component's properties
UReactToProjComponent::UReactToProjComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UReactToProjComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!CollisionComponent)
	{
		// Automatically find the first primitive component with UPrimitiveComponent
		CollisionComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
	}

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UReactToProjComponent::HandleBeginOverlap);

	}
	else
	{

	}
}


// Called every frame
void UReactToProjComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UReactToProjComponent::HandleBeginOverlap(
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

	

	// Check for tag
	if (OtherActor->ActorHasTag(ProjTag))
	{
		
		HitByProjectile(OtherActor);
	}
}

void UReactToProjComponent::HitByProjectile(AActor* projActor)
{
	OnProjHit.Broadcast();

	GetOwner()->Destroy();
	projActor->Destroy();
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "LevelSystem.h"
#include "HazardCollisionComponent.h"

// Sets default values for this component's properties
UHazardCollisionComponent::UHazardCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHazardCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!BoxComponent)
	{
		BoxComponent = GetOwner()->FindComponentByClass<UBoxComponent>();
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UHazardCollisionComponent::HandleBeginOverlap);

	}
}


// Called every frame
void UHazardCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHazardCollisionComponent::HandleBeginOverlap(
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

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		
		ALevelSystem* foundLevelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
		if (foundLevelSystem)
		{
			foundLevelSystem->OnPlayerTouchHazard(OneHitKill, OverrideInvincibility);
		}
	}
}
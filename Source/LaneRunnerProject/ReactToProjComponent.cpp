// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactToProjComponent.h"
#include "DestructibleObjectComponent.h"
#include "OneShotAnim.h"
#include "Projectile.h"
#include "EngineUtils.h"

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

	if (!BoxComponent)
	{
		BoxComponent = GetOwner()->FindComponentByClass<UBoxComponent>();
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UReactToProjComponent::HandleBeginOverlap);
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

	if (OtherComp->ComponentHasTag(ProjTag))
	{
		HitByProjectile(OtherActor);
	}
}

void UReactToProjComponent::HitByProjectile(AActor* projActor)
{
	OnProjHit.Broadcast();

	if (TakeDamageOnHit)
	{
		UDestructibleObjectComponent* destructible = (UDestructibleObjectComponent*)GetOwner()->GetComponentByClass(UDestructibleObjectComponent::StaticClass());
		if (destructible)
		{
			destructible->OnHit();
		}
	}

	bool impactShouldScroll = false;

	if (auto* scrollComp = GetOwner()->GetComponentByClass<UScrollWithPlayerComponent>())
	{
		impactShouldScroll = scrollComp->Enabled;
	}

	if (BlockProj)
	{
		AProjectile* proj = Cast<AProjectile>(projActor);
		proj->OnDestroy(impactShouldScroll);
	}
}

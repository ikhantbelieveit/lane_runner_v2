// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactToProjComponent.h"
#include "DestructibleObjectComponent.h"
#include "OneShotAnim.h"
#include "Projectile.h"
#include "EngineUtils.h"
#include "LocationManagerComponent.h"
#include "GI_AudioSystem.h"
#include "SpawnComponent.h"

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

	UStaticMeshComponent* meshComp = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (meshComp)
	{
		meshComp->OnComponentHit.AddDynamic(this, &UReactToProjComponent::HandleHit);
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

void UReactToProjComponent::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
			if (auto* audioSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_AudioSystem>())
			{
				if (destructible->GetCurrentHealth() <= 0 && destructible->StartHealth > 1)
				{
					audioSystem->Play(EAudioKey::DestroyedByPlayer_Default);
				}
				else
				{
					audioSystem->Play(EAudioKey::ShotByPlayer_Default);
				}
			}
		}
	}

	bool impactShouldScroll = false;
	float scrollWithPlayerOffset = 0.0f;

	if (auto* scrollComp = GetOwner()->GetComponentByClass<ULocationManagerComponent>())
	{
		impactShouldScroll = scrollComp->bScrollEnabled;
		scrollWithPlayerOffset = scrollComp->ScrollWithXPos;
	}

	if (BlockProj)
	{
		AProjectile* proj = Cast<AProjectile>(projActor);
		proj->OnDestroy(impactShouldScroll, scrollWithPlayerOffset);
	}
}
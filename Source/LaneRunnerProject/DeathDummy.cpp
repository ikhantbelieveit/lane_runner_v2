// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathDummy.h"
#include "Projectile.h"
#include "LocationManagerComponent.h"

// Sets default values
ADeathDummy::ADeathDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ADeathDummy::InitFromActor(AActor* SourceActor)
{
    if (!SourceActor) return;


}

// Called when the game starts or when spawned
void ADeathDummy::BeginPlay()
{
	Super::BeginPlay();

    UProjectileMovementComponent* projectileComp = GetComponentByClass<UProjectileMovementComponent>();

    if (LaunchVelocityList.Num() > 0)
    {
        // Pick a random velocity from array
        const FVector LaunchVel = LaunchVelocityList[FMath::RandRange(0, LaunchVelocityList.Num() - 1)];
        projectileComp->Velocity = LaunchVel;
    }
    else
    {
        // Default fallback if no velocities assigned
        projectileComp->Velocity = FVector(
            FMath::FRandRange(-100.f, 100.f),
            FMath::FRandRange(-50.f, 50.f),
            FMath::FRandRange(400.f, 600.f)
        );
    }

    // Optional: auto-destroy after a few seconds
    if (LifeTimeSeconds > 0.0f)
    {
        GetWorldTimerManager().SetTimerForNextTick([this]()
            {
                SetLifeSpan(LifeTimeSeconds);
            });
    }
}


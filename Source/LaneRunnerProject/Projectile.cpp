// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "EmitterComponent.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    bReadyToDestroy = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(OverlapEnableTimer, this, &AProjectile::CheckOverlapOnInit, 0.01f, false);
    }
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bTriggerDestroyWhenReady && bReadyToDestroy)
    {
        OnDestroy(bDelayedDestroy_ScrollWithPlayer, bDelayedDestroy_ScrollOffset);
    }

    FVector NewLocation = GetActorLocation() + Velocity * DeltaTime;
    SetActorLocation(NewLocation);
}

void AProjectile::CheckOverlapOnInit()
{
    bReadyToDestroy = true;

    UBoxComponent* Box = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));

    TArray<UPrimitiveComponent*> Overlaps;
    Box->GetOverlappingComponents(Overlaps);

    for (UPrimitiveComponent* O : Overlaps)
    {
        if (!O) continue;

        if (O->ComponentHasTag("DestroyProj"))
        {
            bool impactScroll = false;
            float scrollOffset = 0;

            ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>();

            if (locManager)
            {
                impactScroll = locManager->bScrollEnabled;
                scrollOffset = locManager->ScrollWithXPos;
            }

            OnDestroy(impactScroll, scrollOffset);
        }
    }
}

void AProjectile::SetupFromConfig()
{
    

    UProjectileMovementComponent* projMoveComp = (UProjectileMovementComponent*)GetComponentByClass(UProjectileMovementComponent::StaticClass());
    UBoxComponent* boxComp = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());
    ULocationManagerComponent* scrollComp = GetComponentByClass<ULocationManagerComponent>();



    if (ConfigData)
    {
        SetLifeSpan(ConfigData->Lifetime);

        switch (FiringDirection)
        {
        case EProjectileDirection::Forward:
            projMoveComp->InitialSpeed = ConfigData->SpeedForward;
            projMoveComp->MaxSpeed = ConfigData->SpeedForward;

            if (scrollComp)
            {
                scrollComp->bScrollEnabled = false;
            }

            //spriteComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
            break;
        case EProjectileDirection::Up:
            //spriteComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

            projMoveComp->InitialSpeed = ConfigData->Speed;
            projMoveComp->MaxSpeed = ConfigData->Speed;

            break;
        default:
            projMoveComp->InitialSpeed = ConfigData->Speed;
            projMoveComp->MaxSpeed = ConfigData->Speed;

            break;
        }

        if (boxComp)
        {
            boxComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::HandleBeginOverlap);
        }
    }
}

void AProjectile::Fire(EProjectileDirection Direction, bool ScrollWithPlayer)
{
    UProjectileMovementComponent* projMoveComp = FindComponentByClass<UProjectileMovementComponent>();
    UEmitterComponent* emitterComp = FindComponentByClass<UEmitterComponent>();

    ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>();

    if (locManager)
    {
        locManager->bScrollEnabled = ScrollWithPlayer;
    }

    FRotator projRotation = FRotator::ZeroRotator;
    FVector directionVector = FVector::ZeroVector;

    switch (Direction)
    {
    case EProjectileDirection::Left:
        directionVector = FVector(0, -1, 0);
        projRotation = FRotator(0.0f, -90.0f, 0.0f);
        break;

    case EProjectileDirection::Right:
        directionVector = FVector(0, 1, 0);
        projRotation = FRotator(0.0f, 90.0f, 0.0f);
        break;

    case EProjectileDirection::Up:
        directionVector = FVector(0, 0, 1);
        projRotation = FRotator(90.0f, 90.0f, 0.0f);
        break;

    case EProjectileDirection::Forward:
        directionVector = FVector(1, 0, 0);
        projRotation = FRotator(0.0f, 0.0f, 90.0f);
        break;

    case EProjectileDirection::Backward:
        directionVector = FVector(-1, 0, 0);
        projRotation = FRotator(180.0f, 0.0f, -90.0f);
        break;
    }

    SetActorRotation(projRotation);

    if (projMoveComp)
    {
        projMoveComp->Velocity = directionVector.GetSafeNormal() * projMoveComp->InitialSpeed;
    }

    if (emitterComp)
    {
        for (FSpawnEntry& Entry : emitterComp->SpawnEntries)
        {
            if (Entry.bInheritOwnerRotation)
            {
                Entry.RotationOffset = FRotator::ZeroRotator; // now inherits from SetActorRotation
                Entry.bScrollInstant = ((Direction != EProjectileDirection::Forward) && (Direction != EProjectileDirection::Backward));
            }
        }
    }

    FiringDirection = Direction;
    
}

EProjectileDirection AProjectile::GetFiringDirection()
{
    return FiringDirection;
}

void AProjectile::SetFiringDirection(EProjectileDirection Direction)
{
    FiringDirection = Direction;
}

void AProjectile::SetProjectileType(EProjectileType Type)
{
    ProjectileType = Type;
}

void AProjectile::OnDestroy(bool impactScroll, float scrollWithPlayerOffset)
{
    if (!bReadyToDestroy)
    {
        bTriggerDestroyWhenReady = true;
        bDelayedDestroy_ScrollOffset = scrollWithPlayerOffset;
        bDelayedDestroy_ScrollWithPlayer = impactScroll;
        return;
    }

    if (ConfigData->ImpactAnimClass)
    {
        float randomRotate = FMath::RandRange(0.0, 360.0);

        FActorSpawnParameters SpawnParams;
        FRotator defaultRotation = FRotator(randomRotate, 90.0f, 0.0f);

        if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->ImpactAnimClass, GetActorLocation(), defaultRotation, SpawnParams))
        {
            if (impactScroll)
            {
                ULocationManagerComponent* scrollComp = impactAnim->GetComponentByClass<ULocationManagerComponent>();
                if (scrollComp)
                {
                    scrollComp->bScrollEnabled = impactScroll;
                    scrollComp->ScrollWithXPos = scrollWithPlayerOffset;
                }
            }
        }
    }

    Destroy();
}

void AProjectile::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == GetOwner()) return;

    if (OtherComp->ComponentHasTag("DestroyProj"))
    {
        bool impactScroll = false;
        float scrollOffset = 0;

        ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>();

        if (locManager)
        {
            impactScroll = locManager->bScrollEnabled;
            scrollOffset = locManager->ScrollWithXPos;
        }

        OnDestroy(impactScroll, scrollOffset);
    }
}
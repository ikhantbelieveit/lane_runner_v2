// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Only create mesh if none exists (allows Blueprint override)
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    ScrollWithPlayerComponent = CreateDefaultSubobject<UScrollWithPlayerComponent>(TEXT("ScrollWithPlayerComponent"));
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation = GetActorLocation() + Velocity * DeltaTime;
    SetActorLocation(NewLocation);
}

void AProjectile::SetupFromConfig()
{
    

    UProjectileMovementComponent* projMoveComp = (UProjectileMovementComponent*)GetComponentByClass(UProjectileMovementComponent::StaticClass());
    UStaticMeshComponent* meshComp = (UStaticMeshComponent*)GetComponentByClass(UStaticMeshComponent::StaticClass());


    if (ConfigData)
    {
        SetLifeSpan(ConfigData->Lifetime);

        switch (FiringDirection)
        {
        case EProjectileDirection::Forward:
            projMoveComp->InitialSpeed = ConfigData->SpeedForward;
            projMoveComp->MaxSpeed = ConfigData->SpeedForward;

            if (meshComp)
            {
                meshComp->SetStaticMesh(ConfigData->ProjectileMesh);
                meshComp->SetRelativeScale3D(FVector(ConfigData->MeshScale));
            }

            if (ScrollWithPlayerComponent)
            {
                ScrollWithPlayerComponent->Enabled = false;
            }
            break;
        default:
            projMoveComp->InitialSpeed = ConfigData->Speed;
            projMoveComp->MaxSpeed = ConfigData->Speed;

            if (meshComp)
            {
                meshComp->SetStaticMesh(ConfigData->ProjectileMesh);
                meshComp->SetRelativeScale3D(FVector(ConfigData->MeshScale));
            }
            break;
        }
        
    }
}

void AProjectile::Fire(EProjectileDirection Direction)
{

    FVector directionVector = FVector(0.0f, 0.0f, 1.0f);

    switch (Direction)
    {
    case EProjectileDirection::Left:
        directionVector = FVector(0, -1, 0);
        break;
    case EProjectileDirection::Right:
        directionVector = FVector(0, 1, 0);
        break;
    case EProjectileDirection::Up:
        directionVector = FVector(0, 0, 1);
        break;
    case EProjectileDirection::Forward:
        directionVector = FVector(1, 0, 0);
        UScrollWithPlayerComponent* scrollComp = (UScrollWithPlayerComponent*)GetComponentByClass(UScrollWithPlayerComponent::StaticClass());
        if (scrollComp)
        {
            scrollComp->Enabled = false;
        }
        
        /*UProjectileMovementComponent* projMoveComp = (UProjectileMovementComponent*)GetComponentByClass(UProjectileMovementComponent::StaticClass());
        projMoveComp->InitialSpeed = 2000.0f;
        projMoveComp->MaxSpeed = 2000.0f;*/
        break;
    }

    if (!ProjectileMovement) return;
    ProjectileMovement->Velocity = directionVector.GetSafeNormal() * ProjectileMovement->InitialSpeed;

    // Optional mesh default if none set in Blueprint
    if (MeshComp && !MeshComp->GetStaticMesh())
    {
        // MeshComp->SetStaticMesh(DefaultMesh);
    }

    
}

EProjectileDirection AProjectile::GetFiringDirection()
{
    return FiringDirection;
}

void AProjectile::SetFiringDirection(EProjectileDirection Direction)
{
    FiringDirection = Direction;
}

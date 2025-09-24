// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;

    // Projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    ScrollWithPlayerComponent = CreateDefaultSubobject<UScrollWithPlayerComponent>(TEXT("ScrollWithPlayerComponent"));

    //PaperSprite Visuals
    SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    SpriteComponent->SetupAttachment(GetRootComponent());
    SpriteComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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
    UPaperSpriteComponent* spriteComp = (UPaperSpriteComponent*)GetComponentByClass(UPaperSpriteComponent::StaticClass());
    UBoxComponent* boxComp = (UBoxComponent*)GetComponentByClass(UBoxComponent::StaticClass());



    if (ConfigData)
    {
        SetLifeSpan(ConfigData->Lifetime);

        switch (FiringDirection)
        {
        case EProjectileDirection::Forward:
            projMoveComp->InitialSpeed = ConfigData->SpeedForward;
            projMoveComp->MaxSpeed = ConfigData->SpeedForward;

            if (ScrollWithPlayerComponent)
            {
                ScrollWithPlayerComponent->Enabled = false;
            }

            spriteComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
            break;
        case EProjectileDirection::Up:
            spriteComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

            projMoveComp->InitialSpeed = ConfigData->Speed;
            projMoveComp->MaxSpeed = ConfigData->Speed;

            break;
        default:
            projMoveComp->InitialSpeed = ConfigData->Speed;
            projMoveComp->MaxSpeed = ConfigData->Speed;

            break;
        }
        
        if (spriteComp)
        {
            spriteComp->SetSprite(ConfigData->ProjSprite);
        }

        if (boxComp)
        {
            if (!boxComp->ComponentHasTag("PlayerProj"))
            {
                boxComp->ComponentTags.Add("PlayerProj");
            }
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
    
}

EProjectileDirection AProjectile::GetFiringDirection()
{
    return FiringDirection;
}

void AProjectile::SetFiringDirection(EProjectileDirection Direction)
{
    FiringDirection = Direction;
}

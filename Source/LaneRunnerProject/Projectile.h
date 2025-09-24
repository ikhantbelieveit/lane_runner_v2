// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/StaticMesh.h"
#include "ProjectileConfigData.h"
#include "EProjectileDirection.h"
#include "ScrollWithPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class LANERUNNERPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
    AProjectile();

    virtual void Tick(float DeltaTime) override;

    void SetupFromConfig();
    void Fire(EProjectileDirection Direction);
    EProjectileDirection GetFiringDirection();
    void SetFiringDirection(EProjectileDirection Direction);

protected:
    virtual void BeginPlay() override;

    EProjectileDirection FiringDirection;

private:
    FVector Velocity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UBoxComponent* BoxComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UScrollWithPlayerComponent* ScrollWithPlayerComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UProjectileConfigData* ConfigData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
    UPaperSpriteComponent* SpriteComponent;

    UPaperSprite* ProjSprite;
};

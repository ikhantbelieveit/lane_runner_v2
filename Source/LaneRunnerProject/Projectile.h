// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/StaticMesh.h"
#include "ProjectileConfigData.h"
#include "EProjectileDirection.h"
#include "LocationManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OneShotAnim.h"
#include "EProjectileType.h"
#include "Projectile.generated.h"

UCLASS()
class LANERUNNERPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
    AProjectile();

    virtual void Tick(float DeltaTime) override;

    void SetupFromConfig();
    void Fire(EProjectileDirection Direction, bool ScrollWithPlayer);
    EProjectileDirection GetFiringDirection();
    void SetFiringDirection(EProjectileDirection Direction);
    void SetProjectileType(EProjectileType Type);

    void OnDestroy(bool impactScroll, float scrollWithPlayerOffset);

protected:
    virtual void BeginPlay() override;

    EProjectileDirection FiringDirection;
    EProjectileType ProjectileType;

    FTimerHandle OverlapEnableTimer;
    bool bReadyToDestroy;
    bool bTriggerDestroyWhenReady;
    bool bDelayedDestroy_ScrollWithPlayer;
    float bDelayedDestroy_ScrollOffset;

private:
    FVector Velocity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UProjectileConfigData* ConfigData;


    UPaperSprite* ProjSprite;

public:
    UFUNCTION()
    void HandleBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void CheckOverlapOnInit();
};

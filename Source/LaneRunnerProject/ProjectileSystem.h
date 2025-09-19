// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameSystem.h"
#include "EProjectileDirection.h"
#include "Projectile.h"
#include "ProjectileSystem.generated.h"

USTRUCT(BlueprintType)
struct FProjectileRequestData
{
	GENERATED_BODY()

public:
	EProjectileDirection Direction;
	TSubclassOf<AProjectile> ProjectileClass;
	FVector ShootPos;
};

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API AProjectileSystem : public ABaseGameSystem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	bool ShootPlayerProjectile(FProjectileRequestData request);

	void ClearAllProjectiles();

	UFUNCTION()
	void OnLevelReset();
};

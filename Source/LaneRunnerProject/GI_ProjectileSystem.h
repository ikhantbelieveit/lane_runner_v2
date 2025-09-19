// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EProjectileDirection.h"
#include "Projectile.h"
#include "GI_ProjectileSystem.generated.h"

USTRUCT(BlueprintType)
struct FProjectileRequestData
{
	GENERATED_BODY()

public:
	EProjectileDirection Direction;
	TSubclassOf<AProjectile> ProjectileClass;
	FVector ShootPos;
};

UCLASS()
class LANERUNNERPROJECT_API UGI_ProjectileSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	bool ShootPlayerProjectile(FProjectileRequestData request);

	void ClearAllProjectiles();

	UFUNCTION()
	void OnLevelReset();
};

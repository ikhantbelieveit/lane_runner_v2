// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EProjectileDirection.h"
#include "Projectile.h"
#include "ProjectileRequestData.h"
#include "GI_ProjectileSystem.generated.h"

UCLASS()
class LANERUNNERPROJECT_API UGI_ProjectileSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ClearAllProjectiles();

	UFUNCTION()
	void OnLevelReset();

	UFUNCTION(BlueprintCallable)
	bool ProcessProjectileRequest(FProjectileRequestData request);

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TMap<EProjectileType, TSubclassOf<class AProjectile>> ProjectileClass_LUT;

};

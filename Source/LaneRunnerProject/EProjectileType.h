// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	None    UMETA(DisplayName = "None"),
	PlayerBullet UMETA(DisplayName = "Player Bullet"),
	TallBanditBullet UMETA(DisplayName = "Tall Bandit Bullet")
};

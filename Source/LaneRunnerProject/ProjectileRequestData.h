// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "EProjectileDirection.h"
#include "EProjectileType.h"
#include "CoreMinimal.h"
#include "ProjectileRequestData.generated.h"

USTRUCT(BlueprintType)
struct FShootItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	EProjectileType Type;

	UPROPERTY(EditAnywhere)
	EProjectileDirection Direction;

	UPROPERTY(EditAnywhere)
	FVector ShootPos;

	UPROPERTY(EditAnywhere)
	FName ShootOriginName;
};

USTRUCT(BlueprintType)
struct FProjectileRequestData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FShootItem> Items;
};

/**
 * 
 */
//class LANERUNNERPROJECT_API ProjectileRequestData
//{
//public:
//	ProjectileRequestData();
//	~ProjectileRequestData();
//};

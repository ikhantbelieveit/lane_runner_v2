// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "ECharacterType.h"
#include "OneShotAnim.h"
#include "PlayerConfigData.generated.h"

class AProjectile;

USTRUCT(BlueprintType)
struct FPlayerCharacterDataItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FString DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* StandingFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpingFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpStartFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPaperFlipbook* JumpLandFlipbook;
};

USTRUCT(BlueprintType)
struct FPlayerCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ECharacterType, FPlayerCharacterDataItem> CharacterDataMap;
};

USTRUCT(BlueprintType)
struct FPlayerInputAssets
{
	GENERATED_BODY()

public:

	// Input mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMap;

	// Input actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_RightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SpeedUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SlowDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootLeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LeftAction_Joystick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_RightAction_Joystick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SpeedUpAction_Joystick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SlowDownAction_Joystick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Continue;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_DebugReset;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Pause;


};

USTRUCT(BlueprintType)
struct FPlayerVisuals
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSprite* PlayerSprite;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	UMaterialInterface* DropShadowMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	UMaterialInterface* SpriteGhostMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	UMaterialInterface* SpriteDefaultMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AOneShotAnim> MuzzleShot_Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AOneShotAnim> MuzzleShot_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AOneShotAnim> MuzzleShot_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AOneShotAnim> MuzzleShot_Forward;
};

USTRUCT(BlueprintType)
struct FPlayerMovement
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float DefaultRunSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float FastRunSpeed = 1200.0f;

	UPROPERTY(EditAnywhere)
	float SlowRunSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
	float JumpRiseGravity = 0.7f;

	UPROPERTY(EditAnywhere)
	float JumpFallGravity = 0.2f;

	UPROPERTY(EditAnywhere)
	float JumpApexHangTime = 0.1f;

	UPROPERTY(EditAnywhere)
	float LaneDistance = 200.0f;

	UPROPERTY(EditAnywhere)
	float GeneralGravityScale = 2.5f;

	UPROPERTY(EditAnywhere)
	float JumpZVelocity = 1000.0f;
};

USTRUCT(BlueprintType)
struct FPlayerMisc
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float CameraHeight = 400.0f;

	UPROPERTY(EditAnywhere)
	float CameraFOV = 110.0f;

	UPROPERTY(EditAnywhere)
	FVector ScrollTriggerBoxScale = FVector(1.0f);

	UPROPERTY(EditAnywhere)
	int StartHealth = 3;

	UPROPERTY(EditAnywhere)
	int MaxHealth = 9;
};

USTRUCT(BlueprintType)
struct FPlayerShoot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float ShootHoldInputDelay = 0.5f;

	UPROPERTY(EditAnywhere)
	int HoldShoot_MaxProjectiles = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
};


/**
 * 
 */
UCLASS(BlueprintType)
class LANERUNNERPROJECT_API UPlayerConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FPlayerInputAssets InputConfig;
	UPROPERTY(EditAnywhere)
	FPlayerVisuals VisualsConfig;
	UPROPERTY(EditAnywhere)
	FPlayerMovement MovementConfig;
	UPROPERTY(EditAnywhere)
	FPlayerShoot ShootConfig;
	UPROPERTY(EditAnywhere)
	FPlayerMisc MiscConfig;
	UPROPERTY(EditAnywhere)
	FPlayerCharacterData CharacterDataConfig;
};

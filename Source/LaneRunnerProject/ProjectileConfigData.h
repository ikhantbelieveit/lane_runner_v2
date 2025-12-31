// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "OneShotAnim.h"
#include "GI_AudioSystem.h"
#include "ProjectileConfigData.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UProjectileConfigData : public UDataAsset
{
	GENERATED_BODY()
public:
    // Visual mesh for the projectile
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    UStaticMesh* ProjectileMesh;

    

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Speed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float SpeedForward = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Lifetime = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class AOneShotAnim> ImpactAnimClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    EAudioKey ImpactSoundKey = EAudioKey::ShotByPlayer_Default;
};

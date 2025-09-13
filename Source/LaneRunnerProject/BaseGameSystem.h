// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SystemConfigData.h"
#include "BaseGameSystem.generated.h"

UENUM(BlueprintType)
enum class EGameSystem : uint8
{
	None       UMETA(DisplayName = "None"),
	LevelSystem    UMETA(DisplayName = "LevelSystem"),
	ProjectileSystem    UMETA(DisplayName = "ProjectileSystem"),
	TestLevelUISystem    UMETA(DisplayName = "TestLevelUISystem"),
	UIStateSystem    UMETA(DisplayName = "UIStateSystem"),
	DeathScreenUISystem    UMETA(DisplayName = "DeathScreenUISystem"),
};

UCLASS()
class LANERUNNERPROJECT_API ABaseGameSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGameSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InitialiseSystem();

	virtual void ResetSystem();

	virtual void SetupFromConfig(USystemConfigData* configData);
};

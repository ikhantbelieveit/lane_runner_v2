// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyDetectBehaviour : uint8
{
	None UMETA(DisplayName = "None"),
	StraightAdvance UMETA(DisplayName = "Straight Advance"),
	Shoot_OneOff UMETA(DisplayName = "Shoot (One-Off)")
};

UCLASS()
class LANERUNNERPROJECT_API ABaseEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPaperSpriteComponent* AlertVFX;
	UPaperSpriteComponent* MainVisuals;

	bool IsAlive;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyDetectBehaviour DetectBehaviour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdvanceSpeed;

	UFUNCTION()
	void OnDetectPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsEnemyAlive();

	UFUNCTION(BlueprintCallable)
	void OnKilled();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* IdleSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* AlertSprite;
};

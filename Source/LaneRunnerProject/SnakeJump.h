// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "SnakeJump.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API ASnakeJump : public ABaseEnemy
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PerformJump();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpVelocity;
	
};

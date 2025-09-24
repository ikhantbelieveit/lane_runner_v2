// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OneShotAnim.generated.h"

UCLASS()
class LANERUNNERPROJECT_API AOneShotAnim : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOneShotAnim();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAnimFinished();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

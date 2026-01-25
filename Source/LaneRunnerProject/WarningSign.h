// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "BullseyeGroup.h"
#include "LocationManagerComponent.h"
#include "WarningSign.generated.h"

UCLASS()
class LANERUNNERPROJECT_API AWarningSign : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarningSign();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TWeakObjectPtr<ABaseEnemy> EnemyRef;

	UPROPERTY()
	TWeakObjectPtr<ABullseyeGroup> GroupRef;

	UPROPERTY()
	TWeakObjectPtr<ULocationManagerComponent> LocationManagerRef;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitialiseSign(FWarningSignData sign);

	UFUNCTION()
	void ResetSign();
	void ClearSign();

};

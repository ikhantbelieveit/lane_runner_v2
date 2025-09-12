// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "DestructibleObjectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UDestructibleObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDestructibleObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool Destroyed;

	int CurrentHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnHit();

	void DestroyFromComp();

	int GetCurrentHealth();

	bool GetIsDestroyed();

	void ResetDestroy();

	UPROPERTY(EditAnywhere)
	int StartHealth = 1;

	ECollisionEnabled::Type DefaultCollMode;

	UFUNCTION()
	void RegisterGameSystemDelegates();

	UFUNCTION()
	void OnLevelReset();

	// New integer property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int PointsGivenOnDestroy = 5;

	// New boolean property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool GivePointsOnDestroy = true;
};

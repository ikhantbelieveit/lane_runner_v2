// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BaseCollectible.generated.h"

UCLASS()
class LANERUNNERPROJECT_API ABaseCollectible : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCollectible();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool Collected;

public:
	bool GetIsCollected();

	void Collect();

	void ResetCollect();

	UFUNCTION()
	void OnLevelReset();

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void Despawn();

	UFUNCTION()
	void Spawn(bool fromDestroyedObject);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PointsValue = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GivePoints = true;

	bool ResetAsSpawned = true;


private:
	FVector StartPos;
};

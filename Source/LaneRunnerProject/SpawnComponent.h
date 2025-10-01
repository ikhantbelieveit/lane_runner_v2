// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API USpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CurrentSpawned;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Despawn();

	UFUNCTION(BlueprintCallable)
	void Spawn(bool drop, bool scrollInstant, bool scrollOnReach);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ResetAsSpawned = true;

	UFUNCTION(BlueprintCallable)
	void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnedCollisionTag = FName("GravAndOverlap");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DespawnedCollisionTag = FName("NoCollision");

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool DropOnSpawn = false;
	//	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool PlayerScrollOnSpawn = false;	//immediately start scrolling in line with the player on spawn

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool ScrollOnPlayerReach = false;	//continue scrolling with the player, once player reaches them
};

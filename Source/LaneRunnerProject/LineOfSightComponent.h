// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LineOfSightComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDetect);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API ULineOfSightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULineOfSightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool DetectsPlayer = false;

public:	

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void OnDetect();

	bool GetDetectsPlayer() const {
		return DetectsPlayer;
	}

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDetect OnDetectPlayer;
};

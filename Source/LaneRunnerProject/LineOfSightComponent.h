// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "EProjectileDirection.h"
#include "LineOfSightComponent.generated.h"

USTRUCT(BlueprintType)
struct FSightZone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ZoneName;

	UBoxComponent* SightBox = nullptr;

	USceneComponent* RayOrigin = nullptr;

	UPROPERTY(EditAnywhere)
	FVector DirectionVector = FVector::ForwardVector;

	UPROPERTY(EditAnywhere)
	EProjectileDirection DirectionEnum = EProjectileDirection::None;

	UPROPERTY(EditAnywhere)
	TArray<FName> ShootProjNames;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDetect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLoseSight);

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool DetectsPlayer = false;

	bool PlayerInSightBox = false;

	FVector SightRayOrigin;

	UBoxComponent* CurrentOccupiedSightBox;

	

	FSightZone* CurrentZone = nullptr;

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

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void OnDetect();

	void LoseSightOfPlayer();

	bool GetDetectsPlayer() const {
		return DetectsPlayer;
	}

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDetect OnDetectPlayer;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLoseSight OnLosePlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRayLength = 500.0f;

	UFUNCTION(BlueprintCallable)
	FName GetActiveSightZoneName() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FSightZone> SightZones;

	UFUNCTION(BlueprintCallable)
	TArray<FName> GetSightZoneProjNames() const;
};

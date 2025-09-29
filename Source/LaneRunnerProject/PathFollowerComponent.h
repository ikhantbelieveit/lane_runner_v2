// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "PathFollowerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UPathFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathFollowerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	AActor* SplineActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	float DefaultSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	float DistanceAlongSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	bool bLoop;

	UFUNCTION(BlueprintCallable, Category = "Spline")
	void SetSpline(USplineComponent* NewSpline);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	bool FollowEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	bool RotateToMatchPath;

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float newSpeed);

	void Reset();

private:
	void FollowSpline(float DeltaTime);

	USplineComponent* CurrentSplineComp;

	float CurrentSpeed;
};

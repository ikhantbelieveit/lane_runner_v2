// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowerComponent.h"
#include "GameFramework/Actor.h"

UPathFollowerComponent::UPathFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPathFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SplineActor)
	{
		USplineComponent* spline = SplineActor->FindComponentByClass<USplineComponent>();
		if (spline)
		{
			CurrentSplineComp = spline;
		}
	}
}

void UPathFollowerComponent::TickComponent(float DeltaTime,ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SplineActor)
	{
		FollowSpline(DeltaTime);
	}
}

void UPathFollowerComponent::FollowSpline(float DeltaTime)
{
	if (!FollowEnabled) return;

	if (!CurrentSplineComp) return;

	const float SplineLength = CurrentSplineComp->GetSplineLength();
	DistanceAlongSpline += Speed * DeltaTime;

	if (bLoop)
	{
		DistanceAlongSpline = FMath::Fmod(DistanceAlongSpline, SplineLength);
		if (DistanceAlongSpline < 0.f)
		{
			DistanceAlongSpline += SplineLength;
		}
	}
	else
	{
		DistanceAlongSpline = FMath::Clamp(DistanceAlongSpline, 0.f, SplineLength);
	}

	// Set actor location and rotation
	FVector Location = CurrentSplineComp->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	GetOwner()->SetActorLocation(Location);

	if (RotateToMatchPath)
	{
		FRotator Rotation = CurrentSplineComp->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		GetOwner()->SetActorRotation(Rotation);
	}
	
}

void UPathFollowerComponent::SetSpline(USplineComponent* NewSpline)
{
	if (NewSpline)
	{
		CurrentSplineComp = NewSpline;
		DistanceAlongSpline = 0.f; // reset when switching spline
	}
}
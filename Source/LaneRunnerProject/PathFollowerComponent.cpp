// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowerComponent.h"
#include "GameFramework/Actor.h"

UPathFollowerComponent::UPathFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPathFollowerComponent::Reset()
{
	SetSpeed(DefaultSpeed);
}

void UPathFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	Direction = 1;

	if (SplineActor)
	{
		USplineComponent* spline = SplineActor->FindComponentByClass<USplineComponent>();
		if (spline)
		{
			CurrentSplineComp = spline;
		}
	}

	Reset();
}

void UPathFollowerComponent::TickComponent(float DeltaTime,ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentSplineComp)
	{
		FollowSpline(DeltaTime);
	}
}

void UPathFollowerComponent::FollowSpline(float DeltaTime)
{
	if (!FollowEnabled) return;

	if (!CurrentSplineComp) return;

	const float SplineLength = CurrentSplineComp->GetSplineLength();
	DistanceAlongSpline += CurrentSpeed * DeltaTime * Direction;

	switch (FollowMode)
	{
	case EPathFollowMode::Loop:
		DistanceAlongSpline = FMath::Fmod(DistanceAlongSpline, SplineLength);
		if (DistanceAlongSpline < 0.f)
		{
			DistanceAlongSpline += SplineLength;
		}
		break;
	case EPathFollowMode::Clamp:
		DistanceAlongSpline = FMath::Clamp(DistanceAlongSpline, 0.f, SplineLength);
		break;
	case EPathFollowMode::PingPong:
		if (DistanceAlongSpline > SplineLength)
		{
			DistanceAlongSpline = SplineLength;
			Direction *= -1;
		}
		else if (DistanceAlongSpline < 0.f)
		{
			DistanceAlongSpline = 0.f;
			Direction *= -1;
		}
		break;
	}
	/*if (bLoop)
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
	}*/

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

void UPathFollowerComponent::SetSpeed(float newSpeed)
{
	CurrentSpeed = newSpeed;
}
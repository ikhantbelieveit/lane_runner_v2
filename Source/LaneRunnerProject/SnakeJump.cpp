// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeJump.h"
#include "TimedActionComponent.h"
#include "Components/BoxComponent.h"

// Called when the game starts or when spawned
void ASnakeJump::BeginPlay()
{
	Super::BeginPlay();

	UTimedActionComponent* actionComponent = GetComponentByClass<UTimedActionComponent>();
	if (actionComponent)
	{
		actionComponent->PerformActionEvent.AddDynamic(this, &ASnakeJump::PerformJump);
	}
	else
	{
	}
}

// Called every frame
void ASnakeJump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnakeJump::PerformJump()
{
	FVector jumpVector = FVector(0.0f, 0.0f, JumpVelocity);

	UBoxComponent* boxComp = GetComponentByClass<UBoxComponent>();
	boxComp->AddImpulse(jumpVector, NAME_None, true);

}
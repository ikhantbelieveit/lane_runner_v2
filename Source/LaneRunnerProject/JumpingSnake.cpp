// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpingSnake.h"
#include "TimedActionComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AJumpingSnake::AJumpingSnake()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJumpingSnake::BeginPlay()
{
	Super::BeginPlay();
	
	UTimedActionComponent* actionComponent = GetComponentByClass<UTimedActionComponent>();
	if (actionComponent)
	{
		actionComponent->PerformActionEvent.AddDynamic(this, &AJumpingSnake::PerformJump);
	}
	else
	{
	}
}

// Called every frame
void AJumpingSnake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJumpingSnake::PerformJump()
{
	FVector jumpVector = FVector(0.0f, 0.0f, JumpVelocity);

	UBoxComponent* boxComp = GetComponentByClass<UBoxComponent>();
	boxComp->AddImpulse(jumpVector, NAME_None, true);

}
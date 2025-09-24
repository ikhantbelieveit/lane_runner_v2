// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpingSnake.h"
#include "TimedActionComponent.h"

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
	actionComponent->PerformActionEvent.AddDynamic(this, &AJumpingSnake::PerformJump);
}

// Called every frame
void AJumpingSnake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJumpingSnake::PerformJump()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("JUMP NOW"));

}
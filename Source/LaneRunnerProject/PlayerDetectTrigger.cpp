// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDetectTrigger.h"

// Sets default values
APlayerDetectTrigger::APlayerDetectTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerDetectTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerDetectTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


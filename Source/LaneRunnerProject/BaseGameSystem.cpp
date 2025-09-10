// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameSystem.h"

// Sets default values
ABaseGameSystem::ABaseGameSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseGameSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGameSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseGameSystem::InitialiseSystem()
{

}

void ABaseGameSystem::ResetSystem()
{

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullseye.h"
#include "LocationManagerComponent.h"
#include "SpawnComponent.h"
#include "PlayerDetectComponent.h"

// Sets default values
ABullseye::ABullseye()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABullseye::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullseye::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullseye::OnAddedToGroup_Implementation(AActor* InGroupActor, ULocationManagerComponent* Manager)
{
    GroupActorRef = InGroupActor;
    GroupManagerRef = Manager;

    UE_LOG(LogTemp, Log, TEXT("%s added to group %s"), *GetName(), *InGroupActor->GetName());
}

void ABullseye::OnRemovedFromGroup_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("%s removed from group"), *GetName());
    GroupActorRef = nullptr;
    GroupManagerRef = nullptr;
}
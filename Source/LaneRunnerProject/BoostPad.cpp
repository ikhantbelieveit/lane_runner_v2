// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostPad.h"
#include "PlayerCharacter.h"

// Sets default values
ABoostPad::ABoostPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoostPad::BeginPlay()
{
	Super::BeginPlay();
	
	if (!BoxComponent)
	{
		BoxComponent = FindComponentByClass<UBoxComponent>();
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoostPad::HandleBeginOverlap);

	}
}

// Called every frame
void ABoostPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoostPad::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);

		if (player)
		{
			player->OnTouchBoostPad(boostSpeed, boostTime);
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockJumpSurface.h"
#include "PlayerCharacter.h"

// Sets default values
ABlockJumpSurface::ABlockJumpSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlockJumpSurface::BeginPlay()
{
	Super::BeginPlay();
	
	if (!BoxComponent)
	{
		BoxComponent = FindComponentByClass<UBoxComponent>();
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlockJumpSurface::HandleBeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ABlockJumpSurface::HandleEndOverlap);
	}
}

void ABlockJumpSurface::HandleBeginOverlap(
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
		auto* player = Cast<APlayerCharacter>(OtherActor);
		player->OnTouchBlockJump();
	}
	else
	{
	}
}

void ABlockJumpSurface::HandleEndOverlap(UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		auto* player = Cast<APlayerCharacter>(OtherActor);
		player->OnExitBlockJump();
	}
	else
	{
	}
}

// Called every frame
void ABlockJumpSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "OneShotAnim.h"
#include "PaperFlipbookComponent.h"

// Sets default values
AOneShotAnim::AOneShotAnim()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AOneShotAnim::BeginPlay()
{
	Super::BeginPlay();

	
	UPaperFlipbookComponent* flipbook = GetComponentByClass<UPaperFlipbookComponent>();

	if (flipbook && flipbook->GetFlipbook())
	{
		flipbook->SetLooping(false);
		flipbook->PlayFromStart();
		flipbook->OnFinishedPlaying.AddDynamic(this, &AOneShotAnim::OnAnimFinished);
	}
}

void AOneShotAnim::OnAnimFinished()
{
	Destroy();
}

// Called every frame
void AOneShotAnim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


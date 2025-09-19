// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "GameInit.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GI_LevelSystem.h"

// Sets default values for this component's properties
UCollectibleComponent::UCollectibleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollectibleComponent::BeginPlay()
{
	Super::BeginPlay();

	UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		DefaultCollMode = box->GetCollisionEnabled();

		box->OnComponentBeginOverlap.AddDynamic(this, &UCollectibleComponent::HandleBeginOverlap);
	}

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UCollectibleComponent::OnLevelReset);
	}
}


// Called every frame
void UCollectibleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCollectibleComponent::GetIsCollected()
{
	return Collected;
}

void UCollectibleComponent::Collect()
{
	if (GetIsCollected())
	{
		return;
	}

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->AddToScore(10);
	}

	UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (mesh)
	{
		mesh->SetVisibility(false);
	}


	UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass());
	if (sprite)
	{
		sprite->SetVisibility(false);
	}

	Collected = true;
}

void UCollectibleComponent::OnLevelReset()
{
	ResetCollect();
}

void UCollectibleComponent::ResetCollect()
{
	if (GetIsCollected())
	{
		UStaticMeshComponent* mesh = (UStaticMeshComponent*)GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass());
		if (mesh)
		{
			mesh->SetVisibility(true);
		}


		UBoxComponent* box = (UBoxComponent*)GetOwner()->GetComponentByClass(UBoxComponent::StaticClass());
		if (box)
		{
			box->SetCollisionEnabled(DefaultCollMode);
		}


		UPaperSpriteComponent* sprite = (UPaperSpriteComponent*)GetOwner()->GetComponentByClass(UPaperSpriteComponent::StaticClass());
		if (sprite)
		{
			sprite->SetVisibility(true);
		}
	}

	Collected = false;
}

void UCollectibleComponent::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	FName PlayerName = "PlayerColl";
	if (OtherComp->ComponentHasTag(PlayerName))
	{
		Collect();
	}
}
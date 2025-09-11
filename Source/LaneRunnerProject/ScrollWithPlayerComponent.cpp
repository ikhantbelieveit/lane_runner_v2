// Fill out your copyright notice in the Description page of Project Settings.


#include "ScrollWithPlayerComponent.h"
#include "GameInit.h"

// Sets default values for this component's properties
UScrollWithPlayerComponent::UScrollWithPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UScrollWithPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	Enabled = StartEnabled;

	UWorld* World = GetWorld();
	if (World)
	{
		AActor* foundPlayerActor = UGameplayStatics::GetActorOfClass(World, APlayerCharacter::StaticClass());

		PlayerRef = Cast<APlayerCharacter>(foundPlayerActor);
		if (nullptr != PlayerRef)
		{
			HasPlayerRef = true;
		}
	}

	AGameInit* gameInit = Cast<AGameInit>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameInit::StaticClass()));
	if (gameInit)
	{
		gameInit->OnAllSystemsSpawned.AddDynamic(this, &UScrollWithPlayerComponent::RegisterGameSystemDelegates);
	}
	
	StartPos = GetOwner()->GetActorLocation();
}


// Called every frame
void UScrollWithPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasPlayerRef && Enabled)
	{
		FVector playerLoc = PlayerRef->GetActorLocation();
		FVector currentLoc = GetOwner()->GetActorLocation();

		FVector newActorLoc = FVector(
			playerLoc.X,
			currentLoc.Y,
			currentLoc.Z);

		GetOwner()->SetActorLocation(newActorLoc);
	}
}

void UScrollWithPlayerComponent::RegisterGameSystemDelegates()
{
	ALevelSystem* levelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UScrollWithPlayerComponent::OnLevelReset);
	}
}

void UScrollWithPlayerComponent::OnLevelReset()
{
	Enabled = StartEnabled;
	GetOwner()->SetActorLocation(StartPos);
}
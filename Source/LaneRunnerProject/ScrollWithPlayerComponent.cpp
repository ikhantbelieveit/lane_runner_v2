// Fill out your copyright notice in the Description page of Project Settings.


#include "ScrollWithPlayerComponent.h"
#include "GameInit.h"
#include "GI_LevelSystem.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"

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

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &UScrollWithPlayerComponent::OnLevelReset);
	}

	StartPos = GetOwner()->GetActorLocation();
}


// Called every frame
void UScrollWithPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasPlayerRef && Enabled)
	{
		FVector currentLoc = GetOwner()->GetActorLocation();
		FVector playerLoc = PlayerRef->GetActorLocation();

		float newXPos = playerLoc.X + ScrollWithXPos;

		FVector newActorLoc = FVector(
			newXPos,
			currentLoc.Y,
			currentLoc.Z);

		if (GetOwner()->SetActorLocation(newActorLoc, false, nullptr, ETeleportType::TeleportPhysics))
		{

		}
	}
}

void UScrollWithPlayerComponent::OnLevelReset()
{
	Enabled = StartEnabled;
	GetOwner()->SetActorLocation(StartPos);
}

void UScrollWithPlayerComponent::SetScrollWithPos(float pos)
{
	ScrollWithXPos = pos;
}

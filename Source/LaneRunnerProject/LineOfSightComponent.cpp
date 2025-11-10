// Fill out your copyright notice in the Description page of Project Settings.


#include "LineOfSightComponent.h"



// Sets default values for this component's properties
ULineOfSightComponent::ULineOfSightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULineOfSightComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UBoxComponent*> boxComps;
	GetOwner()->GetComponents<UBoxComponent>(boxComps);

	TArray<USceneComponent*> sceneComps;
	GetOwner()->GetComponents<USceneComponent>(sceneComps);

	// Build zones from tags
	for (UBoxComponent* box : boxComps)
	{
		if (!box) continue;

		for (const FName& tag : box->ComponentTags)
		{
			FString tagStr = tag.ToString();
			if (tagStr.StartsWith(TEXT("SightBox_")))
			{
				FString zoneName = tagStr.Mid(9); // after SightBox_
				FSightZone& zone = SightZones.FindOrAdd(FName(*zoneName));
				zone.SightBox = box;

				// Connect overlap events
				box->OnComponentBeginOverlap.AddDynamic(this, &ULineOfSightComponent::HandleBeginOverlap);
				box->OnComponentEndOverlap.AddDynamic(this, &ULineOfSightComponent::HandleEndOverlap);
			}
		}
	}

	// Match ray origins to zones
	for (USceneComponent* comp : sceneComps)
	{
		if (!comp) continue;

		for (const FName& tag : comp->ComponentTags)
		{
			FString tagStr = tag.ToString();
			if (tagStr.StartsWith(TEXT("SightRayOrigin_")))
			{
				FString zoneName = tagStr.Mid(15); // after SightRayOrigin_
				if (FSightZone* zone = SightZones.Find(FName(*zoneName)))
				{
					zone->RayOrigin = comp;
				}
			}
		}
	}
}

void ULineOfSightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	if (!CurrentZone || !CurrentZone->SightBox || !PlayerInSightBox)
	{
		return;
	}
		

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Start = CurrentZone->RayOrigin
		? CurrentZone->RayOrigin->GetComponentLocation()
		: GetOwner()->GetActorLocation();

	FVector Forward = CurrentZone->DirectionVector;

	float boxLength = CurrentZone->SightBox->GetScaledBoxExtent().X * 2.0f;
	FVector End = Start + Forward * boxLength;


	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.bTraceComplex = false;

	bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, Params);

	//DrawDebugLine(World, Start, End, FColor::Green, false, 0.0f, 0, 2.0f);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag("Player"))
		{
			OnDetect();

			/*GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Magenta,
				TEXT("PLAYER DETECT")
			);*/
		}
	}
}


void ULineOfSightComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (OtherComp->ComponentHasTag("PlayerColl"))
	{
		for (auto& Pair : SightZones)
		{
			if (Pair.Value.SightBox == OverlappedComp)
			{
				CurrentZone = &Pair.Value;
				PlayerInSightBox = true;

				
				break;
			}
		}
	}
}

void ULineOfSightComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (OtherComp->ComponentHasTag("PlayerColl"))
	{
		if (CurrentZone && CurrentZone->SightBox == OverlappedComp)
		{
			PlayerInSightBox = false;
			CurrentZone = nullptr;
			LoseSightOfPlayer();
		}
	}
}

void ULineOfSightComponent::OnDetect()
{
	if (DetectsPlayer)
	{
		return;
	}

	OnDetectPlayer.Broadcast();

	DetectsPlayer = true;
}

void ULineOfSightComponent::LoseSightOfPlayer()
{
	if (!DetectsPlayer)
	{
		return;
	}

	OnLosePlayer.Broadcast();

	DetectsPlayer = false;
}

FName ULineOfSightComponent::GetActiveSightZoneName() const
{
	if (!CurrentZone)
	{
		return NAME_None;
	}

	// Find which map entry matches CurrentZone
	for (const TPair<FName, FSightZone>& Pair : SightZones)
	{
		if (&Pair.Value == CurrentZone)
		{
			return Pair.Key;
		}
	}

	return NAME_None;
}

TArray<FName> ULineOfSightComponent::GetSightZoneProjNames() const
{
	TArray<FName> returnList = TArray<FName>();

	if (CurrentZone)
	{
		returnList = CurrentZone->ShootProjNames;
	}

	return returnList;
}

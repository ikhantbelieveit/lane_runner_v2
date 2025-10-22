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

	// ...

	UBoxComponent* foundLineOfSight = nullptr;

	TArray<UBoxComponent*> boxComps;

	GetOwner()->GetComponents<UBoxComponent>(boxComps);

	for (UBoxComponent* box : boxComps)
	{
		if (!box)
		{
			continue;
		}

		if (box->ComponentHasTag("LineOfSight"))
		{
			foundLineOfSight = box;
			break;
		}
	}
	

	if (foundLineOfSight)
	{
		LineOfSightBox = foundLineOfSight;
		LineOfSightBox->OnComponentBeginOverlap.AddDynamic(this, &ULineOfSightComponent::HandleBeginOverlap);
		LineOfSightBox->OnComponentEndOverlap.AddDynamic(this, &ULineOfSightComponent::HandleEndOverlap);
	}

	TArray<USceneComponent*> sceneComps;

	GetOwner()->GetComponents<USceneComponent>(sceneComps);

	for (USceneComponent* comp : sceneComps)
	{
		if (!comp)
		{
			continue;
		}

		if (comp->ComponentHasTag("SightRayOrigin"))
		{
			SightRayOrigin = comp->GetComponentTransform().GetLocation();
			break;
		}
	}
}

void ULineOfSightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!PlayerInSightBox)
		return;

	UWorld* World = GetWorld();
	if (!World) return;

	// Start is the stored sight origin location (captured in BeginPlay)
	FVector Start = SightRayOrigin;

	// Direction: use owner's forward vector (world space)
	FVector Forward = GetOwner()->GetActorForwardVector();

	float boxLength = LineOfSightBox->GetScaledBoxExtent().X * 2.0f;
	FVector End = Start - Forward * boxLength;

	// Trace
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()); // ignore the enemy itself
	Params.bTraceComplex = false;

	bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, Params);

	/*FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(World, Start, End, LineColor, false, 0.1f, 0, 1.5f);*/

	// Print what we hit
	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor->ActorHasTag("Player"))
		{
			OnDetect();
		}
		/*FString HitName = HitActor ? HitActor->GetName() : TEXT("ComponentOnlyHit");
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan,
			FString::Printf(TEXT("SightRay Hit: %s"), *HitName));*/
	}
}


void ULineOfSightComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp,
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

	if (OtherComp->ComponentHasTag("PlayerColl"))
	{
		PlayerInSightBox = true;
		//OnDetect();
	}
}

void ULineOfSightComponent::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherComp->ComponentHasTag("PlayerColl"))
	{
		PlayerInSightBox = false;
		LoseSightOfPlayer();
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

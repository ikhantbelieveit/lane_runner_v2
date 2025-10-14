// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorComponent.h"

// Sets default values for this component's properties
UFloorComponent::UFloorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFloorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UFloorComponent::InitialiseFloor(ULevelChunkConfigData* configData)
{
	if (!configData) return;

	int horizontalPos = GetOwner()->GetActorLocation().Y;

	switch (horizontalPos)
	{
	case -400:
		LaneIndex = 0;
		break;
	case -200:
		LaneIndex = 1;
		break;
	case 0:
		LaneIndex = 2;
		break;
	case 200:
		LaneIndex = 3;
		break;
	case 400:
		LaneIndex = 4;
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ERROR - floor has invalid Y pos %d"), horizontalPos));
		break;
	}

	UStaticMeshComponent* meshComp = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (meshComp)
	{
		if (LaneIndex % 2 == 0)
		{
			meshComp->SetMaterial(0, configData->LaneMaterial_1);	//this is line 59
		}
		if (LaneIndex % 2 == 1)
		{
			meshComp->SetMaterial(0, configData->LaneMaterial_2);
		}
	}
}

// Called every frame
void UFloorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelChunkConfigData.h"
#include "FloorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UFloorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFloorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int LaneIndex;

public:	
	// Called every frame
	UFUNCTION(BlueprintCallable)
	void InitialiseFloor(ULevelChunkConfigData* configData);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

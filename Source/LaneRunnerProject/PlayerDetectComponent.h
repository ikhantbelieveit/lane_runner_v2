// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "GI_LevelSystem.h"
#include "LevelChunkActor.h"
#include "PlayerDetectComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UPlayerDetectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerDetectComponent();

protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PlayerAreaTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	TArray<FLevelEventData> EventsToTrigger;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	UBoxComponent* BoxComponent;

public:
	UFUNCTION(BlueprintCallable)
	void ResolveTargetActorIDs(ALevelChunkActor* parentChunk);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Event")
	bool UseParentEventLogic = true;	//optional control over outdated logic from here or new EventTrigger logic
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDetectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UPlayerDetectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerDetectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// The collision component we want to listen on (assignable in Blueprint instance)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	UPrimitiveComponent* CollisionComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TriggerScrollWithPlayer;
};

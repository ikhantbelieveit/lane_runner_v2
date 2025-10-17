// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathDummy.generated.h"


UCLASS()
class LANERUNNERPROJECT_API ADeathDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeathDummy();

	UFUNCTION(BlueprintCallable)
	void InitFromActor(AActor* SourceActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> LaunchVelocityList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTimeSeconds = 3.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

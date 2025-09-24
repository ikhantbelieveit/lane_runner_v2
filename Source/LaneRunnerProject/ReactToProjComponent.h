// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "ReactToProjComponent.generated.h"

// Declare a delegate so you can bind functions to it in Blueprints or C++
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjOverlap);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UReactToProjComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReactToProjComponent();

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

	UPROPERTY(BlueprintAssignable, Category = "Overlap")
	FOnProjOverlap OnProjHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProjTag;

	virtual void HitByProjectile(AActor* projActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TakeDamageOnHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BlockProj = true;

	
};

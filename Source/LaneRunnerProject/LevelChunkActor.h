// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkSpawnEntry.h"
#include "LevelChunkConfigData.h"
#include "LevelLayoutData.h"
#include "Components/BoxComponent.h"
#include "LevelChunkActor.generated.h"



UCLASS()
class LANERUNNERPROJECT_API ALevelChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChunkActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AActor* GetChildActorByID(FName childID, bool& success);

    UFUNCTION(BlueprintCallable)
    void SpawnChunkElements();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelChunkConfigData* ConfigData;

	UFUNCTION(BlueprintCallable)
	void Teardown();

	UFUNCTION(BlueprintCallable)
	void ApplyVariant();


	void InitializeFromLayoutData(const FLevelChunkData& InChunkData);


protected:
	TArray<AActor*> SpawnedActors;

	TMap<FName, FName> ActiveVariants;
	bool IsActorVariantActive(const AActor* Actor) const;
	void DeactivateActor(AActor* Actor) const;
	void ReactivateActor(AActor* Actor) const;

	UBoxComponent* BoundsBox = nullptr;

	UFUNCTION()
	void OnBoundsBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	FName ChunkID;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelLayoutData.h"
#include "MyGameInstance.h"
#include "GI_ChunkManagerSystem.generated.h"

USTRUCT(BlueprintType)
struct FLevelSection
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SectionLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLevelChunkData> Chunks;
};

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_ChunkManagerSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void InitFromLayoutData(FLevelLayoutData layourData);

	UFUNCTION(BlueprintCallable)
	void InitLayoutDataSections();

	UFUNCTION(BlueprintCallable)
	FLevelSection GetSectionForCurrentIndex();

	UFUNCTION(BlueprintCallable)
	void ClearChunks();

	UFUNCTION(BlueprintCallable)
	void SpawnChunksForCurrentSection();

	UFUNCTION()
	void OnPlayerDistanceUpdate(float distance);

	UFUNCTION(BlueprintCallable)
	void OnPlayerSet(APlayerCharacter* player);

protected:
	TArray<ALevelChunkActor*> ActiveChunkActors;
	FLevelLayoutData CurrentLayoutData;
	TArray<FLevelSection> LayoutDataSections;
	const float MinSectionLength = 17500;
	const float SectionEndDistanceThreshold = 5000;
	int CurrentSectionIndex = 0;
	FLevelSection CurrentSection;
	int NumLayoutDataSections;
	float SectionSpawnXPos = 0;
	float NextSectionIncrementThreshold = 0;

	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);

private:
	UMyGameInstance* GI;

	void IncrementSection();
};

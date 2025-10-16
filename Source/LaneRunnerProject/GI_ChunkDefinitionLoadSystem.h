// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelChunkDefinitionAsset.h"
#include "ELevelChunkType.h"
#include "GameUtility.h"
#include "GI_ChunkDefinitionLoadSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_ChunkDefinitionLoadSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool GetChunkDefinition(FName ChunkID, FLevelChunkDefinition& OutDefinition) const
	{
		const FLevelChunkDefinition* FoundDef = ChunkDef_LUT.Find(ChunkID);
		if (FoundDef)
		{
			OutDefinition = *FoundDef;
			return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No chunk definition found for ID"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ChunkID.ToString());
		return false;
	}

	UFUNCTION(BlueprintCallable)
	bool GetRandomDefOfType(ELevelChunkType type, FRandomStream& random, FLevelChunkDefinition& OutDefinition) const
	{
		TArray<FLevelChunkDefinition> filteredChunks = GetAllDefsOfType(type);

		if (filteredChunks.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No chunks found matching given type"));
			return false;
		}

		if (!UGameUtility::GetWeightedRandomItem(filteredChunks, random, OutDefinition))
		{
			//complain here
			return false;
		}

		return true;
	}

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TMap<FName, FLevelChunkDefinition> ChunkDef_LUT;

	TArray<FLevelChunkDefinition> GetAllDefsOfType(ELevelChunkType type) const {

		TArray<FLevelChunkDefinition> returnArr;

		for (const auto& pair : ChunkDef_LUT)
		{
			const FLevelChunkDefinition& def = pair.Value;
			if (def.Type == type)
			{
				returnArr.Add(def);
			}
		}

		return returnArr;
	}
};

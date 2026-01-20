// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ECollectibleType.h"
#include "GameUtility.h"
#include "CollectibleSet.generated.h"


USTRUCT(BlueprintType)
struct FCollectibleSetItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ECollectibleType Type;

	UPROPERTY(EditAnywhere)
	float Weight = 1.0f;

public:
	FORCEINLINE float GetWeight() const { return Weight; }
};

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UCollectibleSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FCollectibleSetItem> Items;

	UFUNCTION(BlueprintCallable)
	bool GetRandomCollectibleType(FRandomStream& random, ECollectibleType& OutType) const
	{
		FCollectibleSetItem item;
		if (!UGameUtility::GetWeightedRandomItem(Items, random, item))
		{
			//complain here
			return false;
		}

		OutType = item.Type;

		return true;
	}
};

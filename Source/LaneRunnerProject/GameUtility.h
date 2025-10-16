// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameUtility.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGameUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    /**
     * Returns a weighted random item from an array.
     * T must have GetWeight() method.
     */
    template<typename T>
    static bool GetWeightedRandomItem(const TArray<T>& Items, FRandomStream& Stream, T& OutItem)
    {
        if (Items.Num() == 0)
            return false;

        float TotalWeight = 0.f;
        for (const T& Item : Items)
        {
            TotalWeight += Item.GetWeight();
        }

        float Pick = Stream.FRandRange(0.f, TotalWeight);
        float Cumulative = 0.f;

        for (const T& Item : Items)
        {
            Cumulative += Item.GetWeight();
            if (Pick <= Cumulative)
            {
                OutItem = Item;
                return true;
            }
        }

        OutItem = Items.Last(); // fallback
        return true;
    }
};

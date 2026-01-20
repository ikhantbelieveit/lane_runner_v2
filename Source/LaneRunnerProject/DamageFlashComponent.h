// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DamageFlashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UDamageFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UDamageFlashComponent();

    UFUNCTION(BlueprintCallable)
    void Initialise();

    UFUNCTION(BlueprintCallable)
    void SetupMaterial(UActorComponent* visualsComp);

    /*UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor FlashColor = FLinearColor::Red;*/

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FlashDuration = 0.2f;

    UFUNCTION(BlueprintCallable)
    void TriggerFlash();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial = nullptr;
    FTimerHandle FlashTimerHandle;

		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_ConfigData.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int TestProperty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGI_ConfigData* ConfigData;

	UFUNCTION(BlueprintCallable)
	bool AllSystemsReady();
};

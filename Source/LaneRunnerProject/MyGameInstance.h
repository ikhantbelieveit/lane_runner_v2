// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_ConfigData.h"
#include "PlayerCharacter.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerSetEvent, APlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldSetEvent, UWorld*, World);

UCLASS()
class LANERUNNERPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void OnStart() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int TestProperty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGI_ConfigData* ConfigData;

	UFUNCTION(BlueprintCallable)
	bool AllSystemsReady();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWorld* WorldRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APlayerCharacter* PlayerRef;

	UPROPERTY(BlueprintAssignable)
	FPlayerSetEvent OnPlayerSet;

	UPROPERTY(BlueprintAssignable)
	FWorldSetEvent OnWorldSet;

	UFUNCTION(BlueprintCallable)
	void SetPlayer(APlayerCharacter* newPlayer);

	UFUNCTION(BlueprintCallable)
	void SetWorld(UWorld* newWorld);
};

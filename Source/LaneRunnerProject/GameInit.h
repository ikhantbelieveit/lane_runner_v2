// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseGameSystem.h"
#include "LevelSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UIStateSystem.h"
#include "GameInitConfigData.h"
#include "GameInit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameInitEvent);

UCLASS()
class LANERUNNERPROJECT_API AGameInit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameInit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<ABaseGameSystem*> SpawnedGameSystems;

	bool HasInitFinished;
	bool HasBroadcastInitFinished;

	void BroadcastInitFinished();

	bool CreateSystemsFromConfigData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameInitEvent OnAllSystemsSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Testing")
	TSubclassOf<UBaseUIScreen> TestUIClass;

	void ShowTestUI();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	UGameInitConfigData* ConfigData;
};

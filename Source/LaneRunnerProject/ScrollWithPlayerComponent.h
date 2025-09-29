// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScrollWithPlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UScrollWithPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScrollWithPlayerComponent();

	APlayerCharacter* PlayerRef;
	bool HasPlayerRef;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool Enabled = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool StartEnabled = true;

	UFUNCTION()
	void OnLevelReset();

	FVector StartPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float ScrollWithXPos;

	UFUNCTION()
	void SetScrollWithPos(float pos);

	UFUNCTION()
	void SetEnabled(bool enabled);

	UFUNCTION()
	bool GetEnabled();
};

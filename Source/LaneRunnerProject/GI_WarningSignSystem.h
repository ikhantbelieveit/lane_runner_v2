// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BullseyeGroup.h"
#include "WarningSign.h"
#include "GI_WarningSignSystem.generated.h"

USTRUCT()
struct FWarningSignPool
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TWeakObjectPtr<AWarningSign>> Inactive;

	UPROPERTY()
	TArray<TWeakObjectPtr<AWarningSign>> Active;
};

UCLASS()
class LANERUNNERPROJECT_API UGI_WarningSignSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void InitialisePools(int prewarmCount);
	void ResetAllPools();

	UFUNCTION()
	void OnLevelReset();

	UFUNCTION(BlueprintCallable)
	bool RequestWarningSign(FWarningSignData signRequestData);

	UFUNCTION(BlueprintCallable)
	void ReturnSignToPool(AWarningSign* sign);

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TSubclassOf<AWarningSign> WarningSignClass;

	FWarningSignPool WarningSignPool;
	AWarningSign* CreateNewWarningSign();

};

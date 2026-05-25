// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelThemeData.h"
#include "GI_LevelThemeDataSystem.generated.h"

/**
 *
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_LevelThemeDataSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool GetThemeData(ELevelTheme ThemeType, FLevelThemeData& OutData) const
	{
		const FLevelThemeData* FoundDef = ThemeData_LUT.Find(ThemeType);
		if (FoundDef)
		{
			OutData = *FoundDef;
			return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to find level theme data"));
		return false;
	}

	UFUNCTION(BlueprintCallable)
	bool GetCurrentThemeData(FLevelThemeData& OutData) const
	{
		return GetThemeData(CurrentTheme, OutData);
	}

	UFUNCTION(BlueprintCallable)
	void SetLevelTheme(ELevelTheme newType);

	UFUNCTION(BlueprintCallable)
	ELevelTheme GetCurrentTheme() const {
		return CurrentTheme;
	}

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TMap<ELevelTheme, FLevelThemeData> ThemeData_LUT;

	ELevelTheme CurrentTheme = ELevelTheme::None;
};

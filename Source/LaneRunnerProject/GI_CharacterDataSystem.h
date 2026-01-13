// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerCharacterData.h"
#include "ECharacterType.h"
#include "GI_CharacterDataSystem.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UGI_CharacterDataSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	bool GetCharacterDefinition(ECharacterType CharacterType, FPlayerCharacterDefinition& OutDefinition) const
	{
		const FPlayerCharacterDefinition* FoundDef = CharacterDef_LUT.Find(CharacterType);
		if (FoundDef)
		{
			OutDefinition = *FoundDef;
			return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to find character definition"));
		return false;
	}

	UFUNCTION(BlueprintCallable)
	TArray<FPlayerCharacterDefinition> GetAllCharacterDefs() const {

		TArray<FPlayerCharacterDefinition> returnArr;

		for (const auto& pair : CharacterDef_LUT)
		{
			const FPlayerCharacterDefinition& def = pair.Value;

			returnArr.Add(def);
		}

		return returnArr;
	}

	UFUNCTION(BlueprintCallable)
	void SetCharacterType(ECharacterType newType);

	UFUNCTION(BlueprintCallable)
	ECharacterType GetCharacterType() const {
		return CurrentCharacterType;
	}

protected:
	bool HasInitialisedFromConfig;
	FTimerHandle TickHandle;
	void TickSubsystem(float DeltaTime);
	bool InitialiseFromConfig();

	TMap<ECharacterType, FPlayerCharacterDefinition> CharacterDef_LUT;

	ECharacterType CurrentCharacterType = ECharacterType::Cowboy_Red;
};

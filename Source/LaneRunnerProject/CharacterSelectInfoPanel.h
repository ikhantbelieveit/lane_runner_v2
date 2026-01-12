// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterType.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectInfoPanel.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class LANERUNNERPROJECT_API UCharacterSelectInfoPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterType CharacterType;

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlavourTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;
	
};

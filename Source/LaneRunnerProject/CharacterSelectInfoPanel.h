// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterType.h"
#include "Components/SizeBox.h"
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

	UFUNCTION(BlueprintCallable)
	void SetNameText(const FText& nameText);

	UFUNCTION(BlueprintCallable)
	void SetFlavourText(const FText& flavourText);

	UFUNCTION(BlueprintCallable)
	void SetPlayerImageTexture(UTexture2D* NewTexture);


protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlavourTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;

	UPROPERTY(meta = (BindWidget))
	USizeBox* ImageSizeBox;
	
};

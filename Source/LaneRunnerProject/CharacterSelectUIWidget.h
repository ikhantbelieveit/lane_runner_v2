// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ECharacterType.h"
#include "CharacterSelectUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LANERUNNERPROJECT_API UCharacterSelectUIWidget : public UBaseUIScreen
{
	GENERATED_BODY()
	
public:
	virtual void Initialise() override;

	virtual void SetupBeforeShow() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UFUNCTION()
	void OnBackButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UFUNCTION()
	void OnConfirmButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UButton* CharacterButton1;

	UFUNCTION()
	void OnCharacterButton1Pressed();

	UPROPERTY(meta = (BindWidget))
	UButton* CharacterButton2;

	UFUNCTION()
	void OnCharacterButton2Pressed();

	UPROPERTY(meta = (BindWidget))
	UButton* CharacterButton3;

	UFUNCTION()
	void OnCharacterButton3Pressed();

	UPROPERTY(meta = (BindWidget))
	UImage* BackButtonArrow;

	UPROPERTY(meta = (BindWidget))
	UImage* ConfirmButtonArrow;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterButton1Arrow;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterButton2Arrow;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterButton3Arrow;

	UPROPERTY(meta = (BindWidget))
	UImage* Character1PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character2PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character3PreviewImage;

	void ShowPreviewImage(ECharacterType characterType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartGameDelay = 0.5f;

	void ToggleConfirmButton(bool active);

private:
	FTimerHandle StartGameDelayHandle;
	void OnStartGameDelayComplete();
};

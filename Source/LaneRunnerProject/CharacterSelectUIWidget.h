// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ECharacterType.h"
#include "UIButtonWidget.h"
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
	virtual void OnScreenShown() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* BackButton;

	UFUNCTION()
	void OnBackButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* ConfirmButton;

	UFUNCTION()
	void OnConfirmButtonPressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton1;

	UFUNCTION()
	void OnCharacterButton1Pressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton2;

	UFUNCTION()
	void OnCharacterButton2Pressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton3;

	UFUNCTION()
	void OnCharacterButton3Pressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton4;

	UFUNCTION()
	void OnCharacterButton4Pressed();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton5;

	UFUNCTION()
	void OnCharacterButton5Pressed();

	UPROPERTY(meta = (BindWidget))
	UImage* Character1PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character2PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character3PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character4PreviewImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Character5PreviewImage;

	void ShowPreviewImage(ECharacterType characterType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartGameDelay = 0.5f;

	void ToggleConfirmButton(bool active);

private:
	FTimerHandle StartGameDelayHandle;
	void OnStartGameDelayComplete();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "ECharacterType.h"
#include "UIButtonWidget.h"
#include "CharacterSelectInfoPanel.h"
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

	virtual void NativeConstruct() override;

	virtual void SetupBeforeShow() override;
	virtual void OnScreenShown() override;
	//virtual void OnScreenHidden() override;

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
	UTextBlock* FlavourTextBlock;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton1;

	UFUNCTION()
	void OnCharacterButton1Pressed();

	UFUNCTION()
	void OnCharFocus1();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton2;

	UFUNCTION()
	void OnCharacterButton2Pressed();

	UFUNCTION()
	void OnCharFocus2();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton3;

	UFUNCTION()
	void OnCharacterButton3Pressed();

	UFUNCTION()
	void OnCharFocus3();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton4;

	UFUNCTION()
	void OnCharacterButton4Pressed();

	UFUNCTION()
	void OnCharFocus4();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton5;

	UFUNCTION()
	void OnCharacterButton5Pressed();

	UFUNCTION()
	void OnCharFocus5();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton6;

	UFUNCTION()
	void OnCharacterButton6Pressed();

	UFUNCTION()
	void OnCharFocus6();

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* CharacterButton7;

	UFUNCTION()
	void OnCharacterButton7Pressed();

	UFUNCTION()
	void OnCharFocus7();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartGameDelay = 0.5f;

	void ToggleConfirmButton(bool active);

private:
	FTimerHandle StartGameDelayHandle;
	void OnStartGameDelayComplete();

	TMap<ECharacterType, TObjectPtr<UCharacterSelectInfoPanel>> InfoPanel_LUT;

	void InitialiseInfoPanelLUT();

	void RefreshShownInfoPanel();

	void SetSelectedCharacter(ECharacterType characterType);
	ECharacterType CurrentSelectedCharacter;
};

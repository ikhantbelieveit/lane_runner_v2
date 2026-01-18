// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUIScreen.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/SizeBox.h"
#include "ECharacterType.h"
#include "UIButtonWidget.h"
#include "GI_CharacterDataSystem.h"
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

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* BackButton;

	UFUNCTION()
	void OnBackButtonPressed(UUIButtonWidget* button);

	UPROPERTY(meta = (BindWidget))
	UUIButtonWidget* ConfirmButton;

	UFUNCTION()
	void OnConfirmButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnCharacterButtonPressed(UUIButtonWidget* button);

	UFUNCTION()
	void OnCharacterButtonFocused(UUIButtonWidget* button);

	UFUNCTION()
	void SetDefaultSelection(UUIButtonWidget* button);

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* CharacterButtonContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUIButtonWidget> CharacterButtonClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCharacterSelectInfoPanel> InfoPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartGameDelay = 0.5f;

	void ToggleConfirmButton(bool active);

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* InfoPanelSwitcher;

	UPROPERTY(meta = (BindWidget))
	USizeBox* InfoPanelSizeBox;

private:
	FTimerHandle StartGameDelayHandle;
	void OnStartGameDelayComplete();

	TMap<ECharacterType, TObjectPtr<UCharacterSelectInfoPanel>> InfoPanel_LUT;

	void InitialiseCharacterEntries();
	void SetupButtonNavigation();

	void RefreshShownInfoPanel();

	void SetSelectedCharacter(ECharacterType characterType);
	ECharacterType CurrentSelectedCharacter;

	TArray<TObjectPtr<UUIButtonWidget>> CharacterButtonList;
	TMap<ECharacterType, TObjectPtr<UUIButtonWidget>> CharacterButton_LUT;

	bool bCharacterEntriesInitialised;

	bool bDidWarmUpLayout = false;
	void WarmUpInfoPanelsLayout();
};

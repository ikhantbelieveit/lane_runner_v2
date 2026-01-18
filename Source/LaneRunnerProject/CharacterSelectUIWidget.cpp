// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectUIWidget.h"
#include "GI_UIStateSystem.h"
#include "GI_LevelSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "GI_CharacterDataSystem.h"
#include "PlayerCharacter.h"

void UCharacterSelectUIWidget::Initialise()
{
	if (ConfirmButton)
	{
		ConfirmButton->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnConfirmButtonPressed);
		ConfirmButton->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::SetDefaultSelection);
	}

	if (BackButton)
	{
		BackButton->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnBackButtonPressed);
		BackButton->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::SetDefaultSelection);
	}
}

void UCharacterSelectUIWidget::InitialiseCharacterEntries()
{
	if (bCharacterEntriesInitialised) { return; }

	InfoPanel_LUT.Empty();
	CharacterButton_LUT.Empty();
	CharacterButtonList.Empty();
	DefaultSelection = nullptr;

	if (!CharacterButtonContainer || !InfoPanelSwitcher)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterSelectUIWidget: Containers not bound. Check your widget BP."));
		return;
	}

	if (!CharacterButtonClass || !InfoPanelClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterSelectUIWidget: CharacterButtonClass / InfoPanelClass not set."));
		return;
	}

	CharacterButtonContainer->ClearChildren();
	InfoPanelSwitcher->ClearChildren();

	UGI_CharacterDataSystem* charDataSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_CharacterDataSystem>();
	if (!charDataSystem) { return; }

	TArray<FPlayerCharacterDefinition> allDefs = charDataSystem->GetAllCharacterDefs();
	if (allDefs.Num() == 0) { return; }

	for (const FPlayerCharacterDefinition& charDef : allDefs)
	{
		ECharacterType charType = charDef.CharacterType;

		//create character button
		UUIButtonWidget* newButton = CreateWidget<UUIButtonWidget>(this, CharacterButtonClass);
		if (!newButton) { continue; }

		newButton->SetLabelText(charDef.DisplayData.PlayerName);
		newButton->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButtonPressed);
		newButton->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButtonFocused);
		newButton->SetPadding(FMargin(0, 0, 0, 20));

		newButton->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::SetDefaultSelection);

		CharacterButtonContainer->AddChild(newButton);

		CharacterButton_LUT.Add(charType, newButton);
		CharacterButtonList.Add(newButton);


		//create info panel
		UCharacterSelectInfoPanel* infoPanel = CreateWidget<UCharacterSelectInfoPanel>(this, InfoPanelClass);
		if (!infoPanel) { continue; }

		infoPanel->CharacterType = charType;
		infoPanel->SetNameText(charDef.DisplayData.PlayerName);
		infoPanel->SetFlavourText(charDef.DisplayData.FlavourText);
		infoPanel->SetPlayerImageTexture(charDef.DisplayData.InfoPanelPortrait);

		InfoPanelSwitcher->AddChild(infoPanel);
		InfoPanel_LUT.Add(charType, infoPanel);
	}

	ForceLayoutPrepass();

	const ECharacterType initialType = ECharacterType::Cowboy_Red;

	SetSelectedCharacter(initialType);
	DefaultSelection = CharacterButtonList[0];

	SetupButtonNavigation();

	bCharacterEntriesInitialised = true;
}

void UCharacterSelectUIWidget::SetupButtonNavigation()
{
	if (!BackButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterSelectUIWidget: BackButton is null; cannot set Left navigation."));
		return;
	}

	int buttonCount = CharacterButtonList.Num();

	for (UUIButtonWidget* button : CharacterButtonList)
	{
		int index = CharacterButtonList.IndexOfByKey(button);

		button->SetNavigationRuleExplicit(EUINavigation::Left, BackButton);

		const int32 UpIndex = (index == 0) ? (buttonCount - 1) : (index - 1);
		if (UUIButtonWidget* UpBtn = CharacterButtonList.IsValidIndex(UpIndex) ? CharacterButtonList[UpIndex] : nullptr)
		{
			button->SetNavigationRuleExplicit(EUINavigation::Up, UpBtn);
		}

		const int32 DownIndex = (index == buttonCount - 1) ? 0 : (index + 1);
		if (UUIButtonWidget* DownBtn = CharacterButtonList.IsValidIndex(DownIndex) ? CharacterButtonList[DownIndex] : nullptr)
		{
			button->SetNavigationRuleExplicit(EUINavigation::Down, DownBtn);
		}
	}

	if (DefaultSelection)
	{
		BackButton->SetNavigationRuleExplicit(EUINavigation::Right, DefaultSelection);
		BackButton->SetNavigationRuleExplicit(EUINavigation::Down, DefaultSelection);
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetNavigationRuleExplicit(EUINavigation::Left, CharacterButtonList[buttonCount - 1]);
		ConfirmButton->SetNavigationRuleExplicit(EUINavigation::Up, CharacterButtonList[buttonCount - 1]);
	}
}

void UCharacterSelectUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCharacterSelectUIWidget::SetupBeforeShow()
{
	if (!bCharacterEntriesInitialised)
	{
		InitialiseCharacterEntries();
	}

	bDidWarmUpLayout = false;

	if (InfoPanelSizeBox)
	{
		InfoPanelSizeBox->SetRenderOpacity(0.0f);
		InfoPanelSizeBox->SetVisibility(ESlateVisibility::Visible);
	}

	ToggleConfirmButton(false);
	SetSelectedCharacter(ECharacterType::Cowboy_Red);
}

void UCharacterSelectUIWidget::OnScreenShown()
{
	CharacterButtonList[0]->SetKeyboardFocus();

	if (!bDidWarmUpLayout)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(this, &UCharacterSelectUIWidget::WarmUpInfoPanelsLayout);
		}
	}
}

void UCharacterSelectUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UCharacterSelectUIWidget::WarmUpInfoPanelsLayout()
{
	if (bDidWarmUpLayout) return;
	bDidWarmUpLayout = true;

	if (!InfoPanelSwitcher)
	{
		return;
	}

	const int32 NumPanels = InfoPanelSwitcher->GetChildrenCount();
	if (NumPanels <= 0)
	{
		return;
	}

	const ECharacterType SavedType = CurrentSelectedCharacter;

	for (int32 i = 0; i < NumPanels; ++i)
	{
		InfoPanelSwitcher->SetActiveWidgetIndex(i);

		InfoPanelSwitcher->ForceLayoutPrepass();
		ForceLayoutPrepass();
	}

	CurrentSelectedCharacter = SavedType;
	RefreshShownInfoPanel();

	InfoPanelSwitcher->ForceLayoutPrepass();
	ForceLayoutPrepass();

	if (InfoPanelSizeBox)
	{
		InfoPanelSizeBox->SetRenderOpacity(1.0f);
	}
}

void UCharacterSelectUIWidget::OnBackButtonPressed(UUIButtonWidget* Button)
{
	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::MainMenu);
	}
}

void UCharacterSelectUIWidget::OnConfirmButtonPressed(UUIButtonWidget* Button)
{
	GetWorld()->GetTimerManager().SetTimer(
		StartGameDelayHandle,
		this,
		&UCharacterSelectUIWidget::OnStartGameDelayComplete,
		StartGameDelay,
		false,
		-1.0f
	);
}

void UCharacterSelectUIWidget::OnCharacterButtonPressed(UUIButtonWidget* button)
{
	for (const auto& Pair : CharacterButton_LUT)
	{
		if (Pair.Value == button)
		{
			ToggleConfirmButton(true);
			SetSelectedCharacter(Pair.Key);
			return;
		}
	}
}

void UCharacterSelectUIWidget::OnCharacterButtonFocused(UUIButtonWidget* button)
{
	for (const auto& Pair : CharacterButton_LUT)
	{
		if (Pair.Value == button)
		{
			SetSelectedCharacter(Pair.Key);
			return;
		}
	}
}

//in case navigation is lost when player clicks outside button
void UCharacterSelectUIWidget::SetDefaultSelection(UUIButtonWidget* button)
{
	DefaultSelection = button;
}

void UCharacterSelectUIWidget::RefreshShownInfoPanel()
{
	if (!InfoPanelSwitcher) { return; }

	if (TObjectPtr<UCharacterSelectInfoPanel>* Found = InfoPanel_LUT.Find(CurrentSelectedCharacter))
	{
		if (UCharacterSelectInfoPanel* Panel = Found->Get())
		{
			InfoPanelSwitcher->SetActiveWidget(Panel);
		}
	}
}

void UCharacterSelectUIWidget::SetSelectedCharacter(ECharacterType characterType)
{
	UGI_CharacterDataSystem* characterDataSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_CharacterDataSystem>();
	if (characterDataSystem)
	{
		characterDataSystem->SetCharacterType(characterType);
	}
	CurrentSelectedCharacter = characterType;
	RefreshShownInfoPanel();
}

void UCharacterSelectUIWidget::OnStartGameDelayComplete()
{
	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::TestLevel);
	}

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->EnterLevel();
	}
}

void UCharacterSelectUIWidget::ToggleConfirmButton(bool active)
{
	if (!ConfirmButton)
	{
		return;
	}

	ConfirmButton->SetVisibility(active ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	for (UUIButtonWidget* Button : CharacterButtonList)
	{
		if (!Button)
		{
			continue;
		}

		if (active)
		{
			Button->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		}
		else
		{
			Button->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		}
	}

	if (active)
	{
		ConfirmButton->SetKeyboardFocus();
	}
}
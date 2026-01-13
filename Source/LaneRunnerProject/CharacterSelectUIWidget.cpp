// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectUIWidget.h"
#include "GI_UIStateSystem.h"
#include "GI_LevelSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "PlayerCharacter.h"

void UCharacterSelectUIWidget::Initialise()
{
	if (ConfirmButton)
	{
		ConfirmButton->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnConfirmButtonPressed);
	}

	if (BackButton)
	{
		BackButton->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnBackButtonPressed);
	}

	if (CharacterButton1)
	{
		DefaultSelection = CharacterButton1;
		CharacterButton1->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton1Pressed);
		CharacterButton1->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus1);
	}

	if (CharacterButton2)
	{
		CharacterButton2->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton2Pressed);
		CharacterButton2->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus2);
	}

	if (CharacterButton3)
	{
		CharacterButton3->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton3Pressed);
		CharacterButton3->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus3);
	}

	if (CharacterButton4)
	{
		CharacterButton4->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton4Pressed);
		CharacterButton4->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus4);
	}

	if (CharacterButton5)
	{
		CharacterButton5->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton5Pressed);
		CharacterButton5->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus5);
	}

	if (CharacterButton6)
	{
		CharacterButton6->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton6Pressed);
		CharacterButton6->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus6);
	}

	if (CharacterButton7)
	{
		CharacterButton7->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton7Pressed);
		CharacterButton7->BroadcastFocusGain.AddDynamic(this, &UCharacterSelectUIWidget::OnCharFocus7);
	}
}

void UCharacterSelectUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitialiseInfoPanelLUT();
	RefreshShownInfoPanel();
}

void UCharacterSelectUIWidget::InitialiseInfoPanelLUT()
{
	InfoPanel_LUT.Empty();

	if (!WidgetTree)
	{
		return;
	}

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);

	TArray<UCharacterSelectInfoPanel*> Panels;
	Panels.Reserve(AllWidgets.Num());

	for (UWidget* W : AllWidgets)
	{
		UCharacterSelectInfoPanel* Panel = Cast<UCharacterSelectInfoPanel>(W);
		if (!Panel)
		{
			continue;
		}

		if (InfoPanel_LUT.Contains(Panel->CharacterType))
		{
			continue;
		}

		InfoPanel_LUT.Add(Panel->CharacterType, Panel);
		Panels.Add(Panel);

		Panel->SetVisibility(ESlateVisibility::Visible);
	}

	ForceLayoutPrepass();
}

void UCharacterSelectUIWidget::SetupBeforeShow()
{
	ToggleConfirmButton(false);
	SetSelectedCharacter(ECharacterType::Cowboy_Red);
}

void UCharacterSelectUIWidget::OnScreenShown()
{
	DefaultSelection = CharacterButton1;
	CharacterButton1->SetKeyboardFocus();
}

void UCharacterSelectUIWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UCharacterSelectUIWidget::OnBackButtonPressed()
{
	auto* uiStateSystem = GetGameInstance()->GetSubsystem<UGI_UIStateSystem>();
	if (uiStateSystem)
	{
		uiStateSystem->EnterScreen(EUIState::MainMenu);
	}
}

void UCharacterSelectUIWidget::OnConfirmButtonPressed()
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

void UCharacterSelectUIWidget::OnCharacterButton1Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Cowboy_Red);
}

void UCharacterSelectUIWidget::OnCharacterButton7Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::ScroogeWife);
}

void UCharacterSelectUIWidget::OnCharFocus1()
{
	SetSelectedCharacter(ECharacterType::Cowboy_Red);
}

void UCharacterSelectUIWidget::OnCharFocus2()
{
	SetSelectedCharacter(ECharacterType::Cowboy_Purple);
}

void UCharacterSelectUIWidget::OnCharFocus3()
{
	SetSelectedCharacter(ECharacterType::Scrooge);
}

void UCharacterSelectUIWidget::OnCharFocus4()
{
	SetSelectedCharacter(ECharacterType::Cow);
}

void UCharacterSelectUIWidget::OnCharFocus5()
{
	SetSelectedCharacter(ECharacterType::Egg);
}

void UCharacterSelectUIWidget::OnCharFocus6()
{
	SetSelectedCharacter(ECharacterType::Postman);
}

void UCharacterSelectUIWidget::OnCharFocus7()
{
	SetSelectedCharacter(ECharacterType::ScroogeWife);
}

void UCharacterSelectUIWidget::OnCharacterButton2Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Cowboy_Purple);
}

void UCharacterSelectUIWidget::OnCharacterButton3Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Scrooge);
}

void UCharacterSelectUIWidget::OnCharacterButton4Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Cow);
}

void UCharacterSelectUIWidget::OnCharacterButton5Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Egg);
}

void UCharacterSelectUIWidget::OnCharacterButton6Pressed()
{
	ToggleConfirmButton(true);
	SetSelectedCharacter(ECharacterType::Postman);
}

void UCharacterSelectUIWidget::RefreshShownInfoPanel()
{
	for (const TPair<ECharacterType, TObjectPtr<UCharacterSelectInfoPanel>>& Pair : InfoPanel_LUT)
	{
		UCharacterSelectInfoPanel* Panel = Pair.Value;
		if (!Panel)
		{
			continue;
		}

		const bool bShouldBeVisible = (Pair.Key == CurrentSelectedCharacter);
		Panel->SetRenderOpacity(bShouldBeVisible ? 1.0f : 0.0f);
	}
}

void UCharacterSelectUIWidget::SetSelectedCharacter(ECharacterType characterType)
{
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

	if (playerActor)
	{
		APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
		if (playerRef)
		{
			playerRef->SetCharacterType(characterType);
		}
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
	if (active)
	{
		ConfirmButton->SetVisibility(ESlateVisibility::Visible);
		ConfirmButton->SetKeyboardFocus();

		CharacterButton1->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton2->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton3->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton4->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton5->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton6->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);
		CharacterButton7->SetNavigationRuleExplicit(EUINavigation::Right, ConfirmButton);

		CharacterButton7->SetNavigationRuleExplicit(EUINavigation::Down, ConfirmButton);
	}
	else
	{
		ConfirmButton->SetVisibility(ESlateVisibility::Hidden);

		CharacterButton1->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton2->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton3->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton4->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton5->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton6->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton7->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);

		CharacterButton7->SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Escape);
	}
}
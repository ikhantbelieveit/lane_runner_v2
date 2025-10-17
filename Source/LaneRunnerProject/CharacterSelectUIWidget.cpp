// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectUIWidget.h"
#include "GI_UIStateSystem.h"
#include "GI_LevelSystem.h"
#include "Kismet/GameplayStatics.h"
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
	}

	if (CharacterButton2)
	{
		CharacterButton2->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton2Pressed);
	}

	if (CharacterButton3)
	{
		CharacterButton3->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton3Pressed);
	}

	if (CharacterButton4)
	{
		CharacterButton4->BroadcastButtonClick.AddDynamic(this, &UCharacterSelectUIWidget::OnCharacterButton4Pressed);
	}
}

void UCharacterSelectUIWidget::SetupBeforeShow()
{
	ToggleConfirmButton(false);

	ShowPreviewImage(ECharacterType::Cowboy_Red);

	
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
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

	if (playerActor)
	{
		APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
		if (playerRef)
		{
			playerRef->SetCharacterType(ECharacterType::Cowboy_Red);
		}
	}

	ToggleConfirmButton(true);

	ShowPreviewImage(ECharacterType::Cowboy_Red);
}

void UCharacterSelectUIWidget::OnCharacterButton2Pressed()
{
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

	if (playerActor)
	{
		APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
		if (playerRef)
		{
			playerRef->SetCharacterType(ECharacterType::Cowboy_Purple);
		}
	}

	ToggleConfirmButton(true);

	ShowPreviewImage(ECharacterType::Cowboy_Purple);
}

void UCharacterSelectUIWidget::OnCharacterButton3Pressed()
{
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

	if (playerActor)
	{
		APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
		if (playerRef)
		{
			playerRef->SetCharacterType(ECharacterType::Scrooge);
		}
	}

	ToggleConfirmButton(true);


	ShowPreviewImage(ECharacterType::Scrooge);
}

void UCharacterSelectUIWidget::OnCharacterButton4Pressed()
{
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());

	if (playerActor)
	{
		APlayerCharacter* playerRef = Cast<APlayerCharacter>(playerActor);
		if (playerRef)
		{
			playerRef->SetCharacterType(ECharacterType::Cow);
		}
	}

	ToggleConfirmButton(true);


	ShowPreviewImage(ECharacterType::Cow);
}

void UCharacterSelectUIWidget::ShowPreviewImage(ECharacterType characterType)
{
	bool showRedCowboy = characterType == ECharacterType::Cowboy_Red;
	bool showPurpleCowboy = characterType == ECharacterType::Cowboy_Purple;
	bool showScrooge = characterType == ECharacterType::Scrooge;
	bool showCow = characterType == ECharacterType::Cow;

	Character1PreviewImage->SetVisibility(showRedCowboy ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Character2PreviewImage->SetVisibility(showPurpleCowboy ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Character3PreviewImage->SetVisibility(showScrooge ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Character4PreviewImage->SetVisibility(showCow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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
		CharacterButton4->SetNavigationRuleExplicit(EUINavigation::Down, ConfirmButton);
	}
	else
	{
		ConfirmButton->SetVisibility(ESlateVisibility::Hidden);

		CharacterButton1->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton2->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton3->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton4->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
		CharacterButton4->SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Escape);
	}
}
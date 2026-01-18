// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectInfoPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UCharacterSelectInfoPanel::SetNameText(const FText& nameText)
{
	NameTextBlock->SetText(nameText);
}

void UCharacterSelectInfoPanel::SetFlavourText(const FText& flavourText)
{
	FlavourTextBlock->SetText(flavourText);
}

void UCharacterSelectInfoPanel::SetPlayerImageTexture(UTexture2D* NewTexture)
{
	CharacterImage->SetBrushFromTexture(NewTexture);

	// Height override is your constant (set in the asset / BP as 300)
	const float Height = ImageSizeBox->GetHeightOverride();
	if (Height <= 0.0f)
	{
		return;
	}

	// Prefer the texture's real dimensions (most reliable)
	float TexW = 0.0f;
	float TexH = 0.0f;

	if (NewTexture)
	{
		TexW = static_cast<float>(NewTexture->GetSizeX());
		TexH = static_cast<float>(NewTexture->GetSizeY());
	}

	// Fallback: brush image size (can be zero in some cases)
	if (TexW <= 0.0f || TexH <= 0.0f)
	{
		TexW = CharacterImage->GetBrush().GetImageSize().X;
		TexH = CharacterImage->GetBrush().GetImageSize().Y;
	}

	if (TexW <= 0.0f || TexH <= 0.0f)
	{
		return;
	}

	const float Aspect = TexW / TexH;
	const float NewWidth = Height * Aspect;

	ImageSizeBox->SetWidthOverride(NewWidth);
}

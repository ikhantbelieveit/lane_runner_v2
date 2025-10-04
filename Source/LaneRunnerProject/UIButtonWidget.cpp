#include "UIButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UUIButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Apply the properties to the bound widgets
	if (LabelTextBlock)
	{
		LabelTextBlock->SetText(LabelText);
	}

	if (ArrowImage)
	{
		if (ArrowTexture)
		{
			ArrowImage->SetBrushFromTexture(ArrowTexture, true);
			ArrowImage->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ArrowImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Optional: hook up the button click if you want to expose clicked event via Blueprint
	// if (RootButton)
	// {
	//     RootButton->OnClicked.AddDynamic(this, &UCustomButtonWidget::YourClickHandler);
	// }
}

void UUIButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (LabelTextBlock)
	{
		LabelTextBlock->SetText(LabelText);
	}

	if (ArrowImage)
	{
		if (ArrowTexture)
		{
			ArrowImage->SetBrushFromTexture(ArrowTexture, true);
			if (ArrowImage->GetVisibility() == ESlateVisibility::Collapsed)
			{
				ArrowImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			ArrowImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UUIButtonWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	ShowArrow(true);
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UUIButtonWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	ShowArrow(false);
	Super::NativeOnFocusLost(InFocusEvent);
}

void UUIButtonWidget::SetLabelText(const FText& NewText)
{
	LabelText = NewText;
	if (LabelTextBlock)
	{
		LabelTextBlock->SetText(LabelText);
	}
}

void UUIButtonWidget::SetArrowTexture(UTexture2D* NewTexture)
{
	ArrowTexture = NewTexture;
	if (ArrowImage)
	{
		if (ArrowTexture)
		{
			ArrowImage->SetBrushFromTexture(ArrowTexture, true);
			ArrowImage->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ArrowImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UUIButtonWidget::ShowArrow(bool bShow)
{
	if (!ArrowImage) return;

	ArrowImage->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
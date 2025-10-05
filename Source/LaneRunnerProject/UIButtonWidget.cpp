#include "UIButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UUIButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

	 if (RootButton)
	 {
	     RootButton->OnClicked.AddDynamic(this, &UUIButtonWidget::OnRootButtonClick);
	 }
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

FReply UUIButtonWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (!InKeyEvent.IsRepeat())
	{
		if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Virtual_Accept)
		{
			if (RootButton)
			{
				OnRootButtonClick();
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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

void UUIButtonWidget::OnRootButtonClick()
{
	BroadcastButtonClick.Broadcast();
}
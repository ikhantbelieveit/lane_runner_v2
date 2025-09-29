// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUIScreen.h"

void UBaseUIScreen::Tick(float DeltaTime)
{
    TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);

    if (!FocusedWidget.IsValid() || FocusedWidget->GetTypeAsString() == TEXT("SViewport"))
    {
        if (DefaultSelection && DefaultSelection->TakeWidget()->SupportsKeyboardFocus())
        {
            DefaultSelection->SetKeyboardFocus();
        }
    }
}

void UBaseUIScreen::OnScreenHidden()
{
    FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);

}
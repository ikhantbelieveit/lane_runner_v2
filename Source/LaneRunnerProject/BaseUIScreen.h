// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUIScreen.generated.h"


UENUM(BlueprintType)
enum class EScreenInputMode : uint8
{
	None UMETA(DisplayName = "None"),
	UIOnly UMETA(DisplayName = "UIOnly"),
	GameAndUI UMETA(DisplayName = "GameAndUI"),
};


UCLASS()
class LANERUNNERPROJECT_API UBaseUIScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void Initialise() {}

	virtual void Tick(float DeltaTime);

	virtual void OnScreenShown() {}
	virtual void OnScreenHidden() {}

	virtual void SetupBeforeShow() {}
	virtual void SetupBeforeHide() {}

	TObjectPtr<UWidget> DefaultSelection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScreenInputMode ScreenInputMode = EScreenInputMode::None;
};

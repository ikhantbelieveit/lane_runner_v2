// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "Camera/CameraComponent.h"
#include "EPlayerSpeedState.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class LANERUNNERPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	// Input mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMap;

	// Input actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_RightAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent;

	/** The sprite asset to assign (set this in the Blueprint defaults) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSprite* PlayerSprite;

	bool LeftInput_Pressed;
	bool RightInput_Pressed;

	EPlayerSpeedState CurrentSpeedState;

	int CurrentLaneIndex;	//0-4 left-right

public:	
	UPROPERTY(EditAnywhere)
	float DefaultRunSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float FastRunSpeed = 1200.0f;

	UPROPERTY(EditAnywhere)
	float SlowRunSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
	float JumpRiseGravity = 0.7f;

	UPROPERTY(EditAnywhere)
	float JumpFallGravity = 0.2f;

	UPROPERTY(EditAnywhere)
	float JumpApexHangTime = 0.1f;

	UPROPERTY(EditAnywhere)
	float LaneDistance = 200.0f;

	UPROPERTY(EditAnywhere)
	float CameraHeight = 175.0f;

	UPROPERTY(EditAnywhere)
	int HoldShoot_MaxProjectiles = 1;

	bool JumpedThisFrame;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//input methods
	void Input_LeftStart();
	void Input_RightStart();

	void ClearInputValues();

	//character update methods
	void UpdateLaneScroll();
	void UpdateLaneFromInput();

	//lane
	bool MoveLane_Left();
	bool MoveLane_Right();
	bool SetLane(int laneIndex);
	bool CanPlayerOccupyLane(int laneIndex);


	//run/scroll
	virtual float GetCurrentRunSpeed();
};

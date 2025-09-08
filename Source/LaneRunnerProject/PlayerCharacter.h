// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerConfigData.h"
#include "EPlayerJumpState.h"
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
#include "GameFramework/CharacterMovementComponent.h"
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

	UInputMappingContext* InputMap;
	UInputAction* Input_LeftAction;
	UInputAction* Input_RightAction;
	UInputAction* Input_SpeedUpAction;
	UInputAction* Input_SlowDownAction;
	UInputAction* Input_ShootRightAction;
	UInputAction* Input_ShootLeftAction;
	UInputAction* Input_ShootUpAction;
	UInputAction* Input_ShootForwardAction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent;

	UPaperSprite* PlayerSprite;

	bool LeftInput_Pressed;
	bool RightInput_Pressed;

	bool SpeedInput_Active;
	bool SpeedInput_Pressed;
	bool SpeedInput_Released;

	bool SlowInput_Active;
	bool SlowInput_Pressed;
	bool SlowInput_Released;

	bool JumpInput_Active;
	bool JumpInput_Pressed;
	bool JumpInput_Released;

	bool ShootLeftInput_Active;
	bool ShootRightInput_Active;
	bool ShootUpInput_Active;
	bool ShootForwardInput_Active;

	bool ShootLeftInput_Pressed;
	bool ShootRightInput_Pressed;
	bool ShootUpInput_Pressed;
	bool ShootForwardInput_Pressed;

	EPlayerSpeedState CurrentSpeedState;

	int CurrentLaneIndex;	//0-4 left-right

	EPlayerJumpState CurrentJumpState;

	

public:	
	float DefaultRunSpeed = 600.0f;

	float FastRunSpeed = 1200.0f;

	float SlowRunSpeed = 200.0f;

	float JumpRiseGravity = 0.7f;

	float JumpFallGravity = 0.2f;

	float JumpApexHangTime = 0.1f;

	float LaneDistance = 200.0f;

	float CameraHeight = 175.0f;

	float CameraFOV = 100.0f;

	int HoldShoot_MaxProjectiles = 1;
	float ShootHoldInputDelay = 0.5f;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void BeginPlay_SetupFromConfig();
	void Input_SetupFromConfig();

	//input methods
	void Input_LeftStart();
	void Input_RightStart();

	virtual void Input_SpeedUpStart(const FInputActionValue& Value);
	virtual void Input_SpeedUp(const FInputActionValue& Value);
	virtual void Input_SpeedUpCancel(const FInputActionValue& Value);

	virtual void Input_SlowDownStart(const FInputActionValue& Value);
	virtual void Input_SlowDown(const FInputActionValue& Value);
	virtual void Input_SlowDownCancel(const FInputActionValue& Value);

	virtual void Input_JumpStart(const FInputActionValue& Value);
	virtual void Input_Jump(const FInputActionValue& Value);
	virtual void Input_JumpCancel(const FInputActionValue& Value);

	void ClearInputValues();

	//character update methods
	void UpdateLaneScroll();
	void UpdateLaneFromInput();
	void UpdateSpeedFromInput();
	void UpdateJumpFromInput();
	void UpdateJumpState(float DeltaTime);
	void UpdateCameraPos();

	//lane
	bool MoveLane_Left();
	bool MoveLane_Right();
	bool SetLane(int laneIndex);
	bool CanPlayerOccupyLane(int laneIndex);


	//run/scroll
	virtual float GetCurrentRunSpeed();
	void SetSpeedState(EPlayerSpeedState newState);


	//jump
	void SetJumpState(EPlayerJumpState newState);
	void CancelVerticalSpeed();

	float TimeSinceJumpStateChange;
	bool JumpedThisFrame;

protected:
	UInputAction* Input_JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UPlayerConfigData* ConfigData;
};

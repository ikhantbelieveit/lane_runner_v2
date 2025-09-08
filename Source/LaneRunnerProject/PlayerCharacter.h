// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	// Input mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMap;

	// Input actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_RightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SpeedUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SlowDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootLeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_ShootDownAction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent;

	/** The sprite asset to assign (set this in the Blueprint defaults) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
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


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_JumpAction;
};

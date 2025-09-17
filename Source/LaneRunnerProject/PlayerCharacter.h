// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/BoxComponent.h"
#include "EProjectileDirection.h"
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
#include "EGameState.h"
#include "SpriteToggleComponent.h"
#include "Components/DecalComponent.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerCharacterEvent);

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

	UInputAction* Input_LeftAction_Joystick;
	UInputAction* Input_RightAction_Joystick;
	UInputAction* Input_SpeedUpAction_Joystick;
	UInputAction* Input_SlowDownAction_Joystick;

	UInputAction* Input_Continue;
	UInputAction* Input_DebugReset;



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

	bool ContinueInput_Pressed;

	bool BlockForwardShoot;

	EPlayerSpeedState CurrentSpeedState;

	int CurrentLaneIndex;	//0-4 left-right

	EPlayerJumpState CurrentJumpState;

	FVector SpawnPos;
	

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
	void Input_LeftStart(const FInputActionValue& Value);
	void Input_RightStart(const FInputActionValue& Value);

	void Input_LeftStart_Joystick(const FInputActionValue& Value);
	void Input_RightStart_Joystick(const FInputActionValue& Value);

	virtual void Input_SpeedUpStart(const FInputActionValue& Value);
	virtual void Input_SpeedUp(const FInputActionValue& Value);
	virtual void Input_SpeedUpCancel(const FInputActionValue& Value);
	virtual void Input_SpeedUpStart_Joystick(const FInputActionValue& Value);
	virtual void Input_SpeedUp_Joystick(const FInputActionValue& Value);
	virtual void Input_SpeedUpCancel_Joystick(const FInputActionValue& Value);

	virtual void Input_SlowDownStart(const FInputActionValue& Value);
	virtual void Input_SlowDown(const FInputActionValue& Value);
	virtual void Input_SlowDownCancel(const FInputActionValue& Value);
	virtual void Input_SlowDownStart_Joystick(const FInputActionValue& Value);
	virtual void Input_SlowDown_Joystick(const FInputActionValue& Value);
	virtual void Input_SlowDownCancel_Joystick(const FInputActionValue& Value);

	virtual void Input_JumpStart(const FInputActionValue& Value);
	virtual void Input_Jump(const FInputActionValue& Value);
	virtual void Input_JumpCancel(const FInputActionValue& Value);

	virtual void Input_ShootLeft(const FInputActionValue& Value);
	virtual void Input_ShootRight(const FInputActionValue& Value);
	virtual void Input_ShootUp(const FInputActionValue& Value);
	virtual void Input_ShootForward(const FInputActionValue& Value);

	virtual void Input_ShootLeftStart(const FInputActionValue& Value);
	virtual void Input_ShootRightStart(const FInputActionValue& Value);
	virtual void Input_ShootUpStart(const FInputActionValue& Value);
	virtual void Input_ShootForwardStart(const FInputActionValue& Value);

	virtual void Input_ContinueStart(const FInputActionValue& Value);
	virtual void Input_ContinueCancel(const FInputActionValue& Value);

	virtual void Input_DebugResetStart(const FInputActionValue& Value);

	void Shoot(EProjectileDirection direction, bool holdNotTap);
	bool CanShootInDirection(EProjectileDirection direction, bool holdNotTap);
	bool DelayPreventsShootInDirection(EProjectileDirection direction);
	bool ExceededProjCountForDirection(EProjectileDirection direction);

	float TimeSinceShoot_Left;
	float TimeSinceShoot_Right;
	float TimeSinceShoot_Up;
	float TimeSinceShoot_Forward;

	int HoldShotsRemaining_Left;
	int HoldShotsRemaining_Right;
	int HoldShotsRemaining_Up;
	int HoldShotsRemaining_Forward;

	void ClearInputValues();

	//character update methods
	void UpdateLaneScroll();
	void UpdateLaneFromInput();
	void UpdateSpeedFromInput();
	void UpdateJumpFromInput();
	void UpdateJumpState(float DeltaTime);
	void UpdateCameraPos();
	void UpdateShootValues(float DeltaTime);
	void UpdateShootFromInput();
	void UpdateCheckForPit();
	void UpdateMercyInvincibility(float DeltaTime);

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

	int StartHealth;
	int CurrentHealth;

	void ResetPlayer();

protected:
	UInputAction* Input_JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UPlayerConfigData* ConfigData;

	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scroll", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ScrollTriggerBox;

public:
	UFUNCTION()
	void RegisterGameSystemDelegates();

	UFUNCTION()
	void OnGameStateChanged(EGameState newState, EGameState prevState);

	void OnHitHazard(bool oneHitKill);
	void SetHealthToMax();

	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth();
	void SetCurrentHealth(int newHealth);

	UPROPERTY(BlueprintAssignable)
	FPlayerCharacterEvent OnHealthSet;

public:
	bool GetMercyInvincibleActive();

protected:
	void StopHorizontalMovement();

	float MercyInvincibleTimeMax = 1.5f;
	bool MercyInvincibleActive;
	float MercyInvincibleTimeLeft;

	void StartMercyInvincibility();
	void CancelMercyInvincibility();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpriteToggleComponent* SpriteToggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent_Ghost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	UDecalComponent* DropShadowDecal;

};

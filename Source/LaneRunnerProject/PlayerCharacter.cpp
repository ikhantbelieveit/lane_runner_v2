// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ScrollWithPlayerComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.h"
#include "PlayerProjectile.h"
#include "GI_LevelSystem.h"
#include "GameInit.h"
#include "GI_ProjectileSystem.h"
#include "GI_AudioSystem.h"
#include "BlockJumpSurface.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(-420.0f, 0.0f, 175.0f)); // eye height
	CameraComponent->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f)); // slight downward tilt
	
	//PaperSprite Visuals
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(GetRootComponent());
	SpriteComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	SpriteComponent_Ghost = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteGhost"));
	SpriteComponent_Ghost->SetupAttachment(GetRootComponent());
	SpriteComponent_Ghost->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	//Scroll Trigger Box
	ScrollTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ScrollTriggerBox"));
	ScrollTriggerBox->SetupAttachment(GetRootComponent());
	ScrollTriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ScrollTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SpriteToggle = CreateDefaultSubobject<USpriteToggleComponent>(TEXT("SpriteToggle"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PlayerCharacter start."));


	//load serialized data from config asset
	BeginPlay_SetupFromConfig();

	//input map
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(InputMap, 0);
		}
	}

	// assign sprite transform
	FRotator DesiredRotation = FRotator(0.0f, 90.0f, 0.0f);
	SpriteComponent->SetWorldRotation(DesiredRotation);
	SpriteComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));

	SpriteComponent_Ghost->SetWorldRotation(DesiredRotation);
	SpriteComponent_Ghost->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));

	ShadowPlaneMesh = FindComponentByTag<UStaticMeshComponent>(TEXT("ShadowPlane"));

	//SpriteToggle->SetSpriteEnabled(FString("Sprite Base"));

	//assign camera FOV
	CameraComponent->SetFieldOfView(CameraFOV);
	

	//set lane to middle
	SetLane(2);

	SpawnPos = GetActorLocation();

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->OnGameStateChanged.AddDynamic(this, &APlayerCharacter::OnGameStateChanged);
		levelSystem->OnLevelRestart.AddDynamic(this, &APlayerCharacter::OnLevelRestart);
		levelSystem->OnLevelExit.AddDynamic(this, &APlayerCharacter::OnLevelExit);
	}

	if (auto* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>())
	{
		flipbookComp->OnFinishedPlaying.AddDynamic(this, &APlayerCharacter::OnFlipbookFinish);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		switch (levelSystem->GetGameState())
		{
		case EGameState::Active:

			UpdateLaneScroll();
			UpdateDistanceTravelled();
			UpdateSpeedFromInput();

			UpdateLaneFromInput();

			UpdateJumpState(DeltaTime);
			UpdateJumpFromInput();

			UpdateShootValues(DeltaTime);
			UpdateShootFromInput();

			UpdateCameraPos();

			UpdateMercyInvincibility(DeltaTime);

			UpdateBoost(DeltaTime);

			UpdateCoyoteTime(DeltaTime);

			UpdateDropShadow();

			UpdateAnimation();

			if (CurrentJumpState == EPlayerJumpState::Grounded)
			{
				LaneMovementBlocked = false;
				if (TouchingBlockJumpSurface)
				{
					LastSurfaceWasBlockJump = true;
				}
				else
				{
					if (IsTouchingSolidFloor())
					{
						LastSurfaceWasBlockJump = false;
					}
				}
			}
			break;
		case EGameState::Lose:
			UpdateCameraPos();
			break;
		case EGameState::AwaitContinue:
			UpdateCameraPos();
			break;
		}
	}

	ClearInputValues();

	JumpedThisFrame = false;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Input_SetupFromConfig();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind actions
		EnhancedInputComponent->BindAction(Input_LeftAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_LeftStart);
		EnhancedInputComponent->BindAction(Input_RightAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_RightStart);
		EnhancedInputComponent->BindAction(Input_LeftAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_LeftStart_Joystick);
		EnhancedInputComponent->BindAction(Input_RightAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_RightStart_Joystick);

		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_SpeedUpStart);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SpeedUp);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SpeedUpCancel);
		/*EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_SpeedUpStart_Joystick);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SpeedUp_Joystick);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SpeedUpCancel_Joystick);*/

		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_SlowDownStart);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SlowDown);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SlowDownCancel);
		/*EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_SlowDownStart_Joystick);
		EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SlowDown_Joystick);
		EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SlowDownCancel_Joystick);*/

		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_JumpStart);
		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_Jump);
		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_JumpCancel);

		EnhancedInputComponent->BindAction(Input_ShootRightAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_ShootRightStart);
		EnhancedInputComponent->BindAction(Input_ShootRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_ShootRight);

		EnhancedInputComponent->BindAction(Input_ShootLeftAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_ShootLeftStart);
		EnhancedInputComponent->BindAction(Input_ShootLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_ShootLeft);

		EnhancedInputComponent->BindAction(Input_ShootUpAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_ShootUpStart);
		EnhancedInputComponent->BindAction(Input_ShootUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_ShootUp);

		EnhancedInputComponent->BindAction(Input_ShootForwardAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_ShootForwardStart);
		EnhancedInputComponent->BindAction(Input_ShootForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_ShootForward);

		EnhancedInputComponent->BindAction(Input_Continue, ETriggerEvent::Started, this, &APlayerCharacter::Input_ContinueStart);
		EnhancedInputComponent->BindAction(Input_Continue, ETriggerEvent::Completed, this, &APlayerCharacter::Input_ContinueCancel);

		EnhancedInputComponent->BindAction(Input_DebugReset, ETriggerEvent::Started, this, &APlayerCharacter::Input_DebugResetStart);
	}

}

void APlayerCharacter::BeginPlay_SetupFromConfig()
{
	if (ConfigData)
	{
		InputMap = ConfigData->InputConfig.InputMap;

		PlayerSprite = ConfigData->VisualsConfig.PlayerSprite;

		if (SpriteComponent)
		{
			if (!SpriteComponent->ComponentHasTag(FName("Sprite Base")))
			{
				SpriteComponent->ComponentTags.Add(FName("Sprite Base"));
			}
		}
		
		if (SpriteComponent_Ghost)
		{
			if (!SpriteComponent_Ghost->ComponentHasTag(FName("Sprite Ghost")))
			{
				SpriteComponent_Ghost->ComponentTags.Add(FName("Sprite Ghost"));
			}
		}
		
		DefaultRunSpeed = ConfigData->MovementConfig.DefaultRunSpeed;
		FastRunSpeed = ConfigData->MovementConfig.FastRunSpeed;
		SlowRunSpeed = ConfigData->MovementConfig.SlowRunSpeed;
		JumpRiseGravity = ConfigData->MovementConfig.JumpRiseGravity;
		JumpFallGravity = ConfigData->MovementConfig.JumpFallGravity;
		JumpApexHangTime = ConfigData->MovementConfig.JumpApexHangTime;
		LaneDistance = ConfigData->MovementConfig.LaneDistance;

		GetCharacterMovement()->GravityScale = ConfigData->MovementConfig.GeneralGravityScale;
		GetCharacterMovement()->JumpZVelocity = ConfigData->MovementConfig.JumpZVelocity;

		CameraHeight = ConfigData->MiscConfig.CameraHeight;
		CameraFOV = ConfigData->MiscConfig.CameraFOV;

		ShootHoldInputDelay = ConfigData->ShootConfig.ShootHoldInputDelay;
		ShootTapInputDelay = ConfigData->ShootConfig.ShootTapInputDelay;
		HoldShoot_MaxProjectiles = ConfigData->ShootConfig.HoldShoot_MaxProjectiles;

		ProjectileClass = ConfigData->ShootConfig.ProjectileClass;

		ScrollTriggerBox->SetRelativeScale3D(FVector(0.1f, 30.0f, 30.0f));
		if (ScrollTriggerBox)
		{
			FString boxSize = ScrollTriggerBox->GetRelativeScale3D().ToString();
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, boxSize);
		}

		FName playerAreaTag = FName("PlayerArea");

		if (!ScrollTriggerBox->ComponentTags.Contains(playerAreaTag))
		{
			ScrollTriggerBox->ComponentTags.Add(playerAreaTag);
		}


		FName playerCollTag = FName("PlayerColl");
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		if (CapsuleComp)
		{
			if (!CapsuleComp->ComponentTags.Contains(playerCollTag))
			{
				CapsuleComp->ComponentTags.Add(playerCollTag);
			}
		}

		StartHealth = ConfigData->MiscConfig.StartHealth;
		MaxHealth = ConfigData->MiscConfig.MaxHealth;

		UPaperFlipbookComponent* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>();
		if (flipbookComp)
		{
			flipbookComp->SetMaterial(0, ConfigData->VisualsConfig.SpriteDefaultMaterial);
		}

	}
	else
	{

	}
}

void APlayerCharacter::Input_SetupFromConfig()
{
	if (ConfigData)
	{
		Input_LeftAction = ConfigData->InputConfig.Input_LeftAction;
		Input_RightAction = ConfigData->InputConfig.Input_RightAction;
		Input_LeftAction_Joystick = ConfigData->InputConfig.Input_LeftAction_Joystick;
		Input_RightAction_Joystick = ConfigData->InputConfig.Input_RightAction_Joystick;
		Input_SpeedUpAction = ConfigData->InputConfig.Input_SpeedUpAction;
		Input_SlowDownAction = ConfigData->InputConfig.Input_SlowDownAction;
		Input_SpeedUpAction_Joystick = ConfigData->InputConfig.Input_SpeedUpAction_Joystick;
		Input_SlowDownAction_Joystick = ConfigData->InputConfig.Input_SlowDownAction_Joystick;
		Input_ShootRightAction = ConfigData->InputConfig.Input_ShootRightAction;
		Input_ShootLeftAction = ConfigData->InputConfig.Input_ShootLeftAction;
		Input_ShootUpAction = ConfigData->InputConfig.Input_ShootUpAction;
		Input_ShootForwardAction = ConfigData->InputConfig.Input_ShootForwardAction;
		Input_JumpAction = ConfigData->InputConfig.Input_JumpAction;

		Input_Continue = ConfigData->InputConfig.Input_Continue;
		Input_DebugReset = ConfigData->InputConfig.Input_DebugReset;

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up input data."));
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up input data EPIC FAIL."));
	}

}

void APlayerCharacter::Input_LeftStart(const FInputActionValue& Value)
{
	LeftInput_Pressed = true;
}

void APlayerCharacter::Input_LeftStart_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis < 0)
	{
		LeftInput_Pressed = true;
	}
}

void APlayerCharacter::Input_RightStart(const FInputActionValue& Value)
{
	RightInput_Pressed = true;
}

void APlayerCharacter::Input_RightStart_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis > 0)
	{
		RightInput_Pressed = true;
	}
}

void APlayerCharacter::Input_SpeedUp(const FInputActionValue& Value)
{
	SpeedInput_Active = true;
}

void APlayerCharacter::Input_SpeedUpStart(const FInputActionValue& Value)
{
	SpeedInput_Pressed = true;
}

void APlayerCharacter::Input_SpeedUpCancel(const FInputActionValue& Value)
{
	SpeedInput_Released = true;
}

void APlayerCharacter::Input_SpeedUp_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis > 0)
	{
		SpeedInput_Active = true;
	}
}

void APlayerCharacter::Input_SpeedUpStart_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	
	if (axis > 0)
	{
		SpeedInput_Pressed = true;
	}
}

void APlayerCharacter::Input_SpeedUpCancel_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis > 0)
	{
		SpeedInput_Released = true;
	}
}

void APlayerCharacter::Input_SlowDownStart(const FInputActionValue& Value)
{
	SlowInput_Pressed = true;
}

void APlayerCharacter::Input_SlowDown(const FInputActionValue& Value)
{
	SlowInput_Active = true;
}

void APlayerCharacter::Input_SlowDownCancel(const FInputActionValue& Value)
{
	SlowInput_Released = true;
}

void APlayerCharacter::Input_SlowDownStart_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	
	if (axis < 0)
	{
		SlowInput_Pressed = true;
	}
}

void APlayerCharacter::Input_SlowDown_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis < 0)
	{
		SlowInput_Active = true;
	}
}

void APlayerCharacter::Input_SlowDownCancel_Joystick(const FInputActionValue& Value)
{
	float axis = Value.Get<float>();
	if (axis < 0)
	{
		SlowInput_Released = true;
	}
}

void APlayerCharacter::Input_JumpStart(const FInputActionValue& Value)
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		if (levelSystem->GetGameState() == EGameState::Active)
		{
			JumpInput_Pressed = true;
		}
	}
}

void APlayerCharacter::Input_Jump(const FInputActionValue& Value)
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		if (levelSystem->GetGameState() == EGameState::Active)
		{
			JumpInput_Active = true;
		}
	}
}

void APlayerCharacter::Input_JumpCancel(const FInputActionValue& Value)
{
	JumpInput_Released = true;
}

void APlayerCharacter::Input_ShootLeft(const FInputActionValue& Value)
{
	ShootLeftInput_Active = true;
}

void APlayerCharacter::Input_ShootRight(const FInputActionValue& Value)
{
	ShootRightInput_Active = true;
}

void APlayerCharacter::Input_ShootUp(const FInputActionValue& Value)
{
	ShootUpInput_Active = true;
}

void APlayerCharacter::Input_ShootForward(const FInputActionValue& Value)
{
	ShootForwardInput_Active = true;
}

void APlayerCharacter::Input_ShootLeftStart(const FInputActionValue& Value)
{
	ShootLeftInput_Pressed = true;
}

void APlayerCharacter::Input_ShootRightStart(const FInputActionValue& Value)
{
	ShootRightInput_Pressed = true;
}

void APlayerCharacter::Input_ShootUpStart(const FInputActionValue& Value)
{
	ShootUpInput_Pressed = true;
}

void APlayerCharacter::Input_ShootForwardStart(const FInputActionValue& Value)
{
	ShootForwardInput_Pressed = true;
}

void APlayerCharacter::Input_ContinueStart(const FInputActionValue& Value)
{
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		if (levelSystem->GetGameState() == EGameState::AwaitContinue)
		{
			ContinueInput_Pressed = true;
			BlockForwardShoot = true;
			levelSystem->TriggerContinue();
		}
	}
}

void APlayerCharacter::Input_ContinueCancel(const FInputActionValue& Value)
{
	BlockForwardShoot = false;
}

void APlayerCharacter::Input_DebugResetStart(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("SHOULD DEBUG RESET"));
}


void APlayerCharacter::ClearInputValues()
{
	LeftInput_Pressed = false;
	RightInput_Pressed = false;

	SpeedInput_Active = false;
	SpeedInput_Pressed = false;
	SpeedInput_Released = false;

	SlowInput_Active = false;
	SlowInput_Pressed = false;
	SlowInput_Released = false;

	JumpInput_Pressed = false;
	JumpInput_Released = false;
	JumpInput_Active = false;

	ShootLeftInput_Active = false;
	ShootRightInput_Active = false;
	ShootUpInput_Active = false;
	ShootForwardInput_Active = false;

	ShootLeftInput_Pressed = false;
	ShootRightInput_Pressed = false;
	ShootUpInput_Pressed = false;
	ShootForwardInput_Pressed = false;

	ContinueInput_Pressed = false;
}

void APlayerCharacter::UpdateLaneScroll()
{
	AddMovementInput(FVector(GetCurrentRunSpeed(), 0.0f, 0.0f));
}

void APlayerCharacter::UpdateLaneFromInput()
{
	if (LaneMovementBlocked)
	{
		return;
	}

	if (LeftInput_Pressed)
	{
		if (SolidBlockingLeftLane())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Erm, left movement denied?"));
		}
		else
		{
			MoveLane_Left();
		}
	}

	if (RightInput_Pressed)
	{
		if (SolidBlockingRightLane())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Erm, right movement denied?"));
		}
		else
		{
			MoveLane_Right();
		}
	}
}

void APlayerCharacter::UpdateSpeedFromInput()
{
	EPlayerSpeedState newState = EPlayerSpeedState::Default;

	if (SpeedInput_Active)
	{
		newState = EPlayerSpeedState::Fast;
	}

	if (SlowInput_Active)
	{
		//newState = EPlayerSpeedState::Slow;
	}

	if (SpeedInput_Active && SlowInput_Active)
	{
		newState = EPlayerSpeedState::Default;
	}

	SetSpeedState(newState);

	if (IsBoosting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BoostOverrideSpeed;
	}
	else
	{
		switch (CurrentSpeedState)
		{
		case EPlayerSpeedState::Fast:
			GetCharacterMovement()->MaxWalkSpeed = FastRunSpeed;
			break;
		case EPlayerSpeedState::Slow:
			GetCharacterMovement()->MaxWalkSpeed = SlowRunSpeed;
			break;
		default:
			GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
			break;
		}
	}
}

void APlayerCharacter::SetSpeedState(EPlayerSpeedState newState)
{
	CurrentSpeedState = newState;
}

void APlayerCharacter::SetJumpState(EPlayerJumpState newState)
{
	if (newState == CurrentJumpState)
	{
		return;
	}

	CurrentJumpState = newState;
	TimeSinceJumpStateChange = 0.0f;
	
	UCharacterMovementComponent* characterMovement = (UCharacterMovementComponent*)GetComponentByClass(UCharacterMovementComponent::StaticClass());

	switch (CurrentJumpState)
	{
	case EPlayerJumpState::Rise:
		
		characterMovement->GravityScale = JumpRiseGravity;
		break;
	case EPlayerJumpState::Apex:
		CancelVerticalSpeed();
		characterMovement->GravityScale = 0;
		break;
	case EPlayerJumpState::Fall:
		characterMovement->GravityScale = JumpFallGravity;
		break;
	case EPlayerJumpState::Grounded:
		characterMovement->GravityScale = JumpRiseGravity;
		HasJumpAvailable = true;
		TimeSinceLeftGround = 0.0f;
		break;
	}
}

void APlayerCharacter::DebugPrintJumpState()
{
	switch (CurrentJumpState)
	{
	case EPlayerJumpState::Rise:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Rise"));
		break;
	case EPlayerJumpState::Apex:
		CancelVerticalSpeed();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Apex"));
		break;
	case EPlayerJumpState::Fall:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Fall"));
		break;
	case EPlayerJumpState::Grounded:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grounded"));
		break;
	}
}

void APlayerCharacter::CancelVerticalSpeed()
{
	GetRootComponent()->ComponentVelocity = FVector(GetRootComponent()->ComponentVelocity.X, GetRootComponent()->ComponentVelocity.Y, 0);
}

void APlayerCharacter::ResetPlayer()
{
	//set lane to middle
	SetLane(2);

	//Move back to start
	SetActorLocation(SpawnPos);
	UpdateCameraPos();

	ResetHealth();

	CancelMercyInvincibility();
	
	ClearInputValues();	//maybe not needed? figured its handy

	CancelBoost();

	TouchingBlockJumpSurface = false;
	LaneMovementBlocked = false;
	HasJumpAvailable = true;
	LastSurfaceWasBlockJump = false;


	SetJumpState(EPlayerJumpState::Grounded);

	SetAnimState(ECharacterAnimState::Grounded);

	DistanceTravelled = 0.0f;
	LastFramePos = SpawnPos;
}

void APlayerCharacter::SetAnimState(ECharacterAnimState newState)
{
	if (CurrentAnimState == newState) return;

	CurrentAnimState = newState;

	switch (CurrentAnimState)
	{
	case ECharacterAnimState::Grounded:
		SetFlipbookVisuals(Flipbook_Stand);
		break;
	case ECharacterAnimState::JumpLoop:
		SetFlipbookVisuals(Flipbook_Jump);
		break;
	case ECharacterAnimState::JumpStart:
		SetFlipbookVisuals(Flipbook_JumpStart);
		break;
	case ECharacterAnimState::JumpLand:
		SetFlipbookVisuals(Flipbook_JumpLand);
	}
}

void APlayerCharacter::OnGameStateChanged(EGameState newState, EGameState prevState)
{
	switch (newState)
	{
	case EGameState::Active:
		switch (prevState)
		{
		case EGameState::Lose:
			ResetPlayer();
			break;
		case EGameState::AwaitContinue:
			StartMercyInvincibility();
		}
		break;
	case EGameState::Dormant:
		break;
	case EGameState::Lose:
		StopHorizontalMovement();
		break;
	case EGameState::Win:
		break;
	case EGameState::AwaitContinue:
		StopHorizontalMovement();
	}
}

void APlayerCharacter::OnHitHazard(bool oneHitKill)
{
	bool resultInKill = oneHitKill ? true : (CurrentHealth <= 1);
	if (!resultInKill)
	{
		auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
		if (audioSystem)
		{
			audioSystem->Play(EAudioKey::TakeDamage);
		}
	}


	if (CurrentHealth > 0)
	{
		if (oneHitKill)
		{
			SetCurrentHealth(0);
		}
		else
		{
			SetCurrentHealth(CurrentHealth - 1);
			StartMercyInvincibility();
		}
	}
}

void APlayerCharacter::SetHealthToMax()
{
	SetCurrentHealth(MaxHealth);
}

void APlayerCharacter::ResetHealth()
{
	SetCurrentHealth(StartHealth);
}

void APlayerCharacter::SetCurrentHealth(int newHealth)
{
	CurrentHealth = newHealth;

	OnHealthSet.Broadcast();
}

int APlayerCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}

bool APlayerCharacter::GetMercyInvincibleActive()
{
	return MercyInvincibleActive;
}

void APlayerCharacter::StopHorizontalMovement()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		FVector Vel = MoveComp->Velocity;
		Vel.X = 0.f;  // Forward/backward
		Vel.Y = 0.f;  // Right/left
		MoveComp->Velocity = Vel;
	}
}

void APlayerCharacter::StartMercyInvincibility()
{
	if (MercyInvincibleActive)
		return;

	MercyInvincibleTimeLeft = MercyInvincibleTimeMax;
	MercyInvincibleActive = true;

	MercyInvincibleFlickerTimer = 0.0f;
	bMercyFlickerVisible = true;

	if (UPaperFlipbookComponent* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>())
	{
		flipbookComp->SetMaterial(0, ConfigData->VisualsConfig.SpriteDefaultMaterial);
		flipbookComp->SetVisibility(true);
	}
}

void APlayerCharacter::CancelMercyInvincibility()
{
	MercyInvincibleTimeLeft = 0.0f;
	MercyInvincibleActive = false;

	if (UPaperFlipbookComponent* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>())
	{
		flipbookComp->SetMaterial(0, ConfigData->VisualsConfig.SpriteDefaultMaterial);
		flipbookComp->SetVisibility(true);
	}
}

void APlayerCharacter::StartBoost(float boostSpeed, float timeToBlockScroll)
{
	BoostOverrideSpeed = boostSpeed;
	BoostTimeRemaining = timeToBlockScroll;
	IsBoosting = true;
}

void APlayerCharacter::CancelBoost()
{
	IsBoosting = false;
}

void APlayerCharacter::OnTouchBoostPad(float boostSpeed, float boostTime)
{
	auto* levelSystem = GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		if (levelSystem->GetGameState() == EGameState::Active)
		{
			FVector boostVector = FVector(boostSpeed, 0.0f, 0.0f);

			StartBoost(boostSpeed, boostTime);
		}
	}
}

void APlayerCharacter::OnTouchBlockJump()
{
	TouchingBlockJumpSurface = true;	
}

void APlayerCharacter::OnExitBlockJump()
{
	if (TouchingBlockJumpSurface)
	{
		if (!IsTouchingBlockJumpFloor())
		{
			TouchingBlockJumpSurface = false;
		}
	}
}

void APlayerCharacter::SetCharacterType(ECharacterType type)
{
	FPlayerCharacterDataItem* data = ConfigData->CharacterDataConfig.CharacterDataMap.Find(type);

	if (data)
	{
		Flipbook_Stand = data->StandingFlipbook;
		Flipbook_Jump = data->JumpingFlipbook;
		Flipbook_JumpStart = data->JumpStartFlipbook;
		Flipbook_JumpLand = data->JumpLandFlipbook;

		CharacterType = type;

		SetFlipbookVisuals(Flipbook_Stand);
		RefreshVisualScale();
	}
}

void APlayerCharacter::RefreshVisualScale()
{
	UPaperFlipbookComponent* FlipbookComp = GetComponentByClass<UPaperFlipbookComponent>();

	float LowestZ = 0.f;
	bool bHasBounds = false;

	for (int32 i = 0; i < Flipbook_Stand->GetNumFrames(); ++i)
	{
		if (UPaperSprite* Sprite = Flipbook_Stand->GetSpriteAtFrame(i))
		{
			FBoxSphereBounds Bounds = Sprite->GetRenderBounds();
			const float SpriteMinZ = Bounds.Origin.Z - Bounds.BoxExtent.Z;
			if (!bHasBounds || SpriteMinZ < LowestZ)
			{
				LowestZ = SpriteMinZ;
				bHasBounds = true;
			}
		}
	}

	if (!bHasBounds) return;

	const FVector Scale = FlipbookComp->GetRelativeScale3D();
	const float ScaledLowestZ = LowestZ * Scale.Z;

	float CapsuleHalfHeight = 0.f;
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	}

	FVector RelLoc = FlipbookComp->GetRelativeLocation();
	RelLoc.Z = -ScaledLowestZ - CapsuleHalfHeight + 5.0f;
	FlipbookComp->SetRelativeLocation(RelLoc);
}

void APlayerCharacter::SetFlipbookVisuals(UPaperFlipbook* flipbook)
{
	UPaperFlipbookComponent* FlipbookComp = GetComponentByClass<UPaperFlipbookComponent>();
	if (!FlipbookComp || !flipbook) return;

	FlipbookComp->SetFlipbook(flipbook);


	FlipbookComp->SetLooping(flipbook != Flipbook_JumpStart && flipbook != Flipbook_JumpLand);
	FlipbookComp->PlayFromStart();
}

void APlayerCharacter::TryAddHealth(int addHealth)
{
	int newHealth = CurrentHealth + addHealth;

	if (newHealth > MaxHealth)
	{
		newHealth = MaxHealth;
	}

	SetCurrentHealth(newHealth);
}


bool APlayerCharacter::MoveLane_Left()
{
	return (SetLane(CurrentLaneIndex - 1));
}

bool APlayerCharacter::MoveLane_Right()
{
	return (SetLane(CurrentLaneIndex + 1));
}

bool APlayerCharacter::SetLane(int laneIndex)
{
	if (!CanPlayerOccupyLane(laneIndex))
	{
		return false;
	}

	bool moveLeft = false;
	bool moveRight = false;

	if (laneIndex == CurrentLaneIndex - 1)
	{
		moveLeft = true;
	}

	if (laneIndex == CurrentLaneIndex + 1)
	{
		moveRight = true;
	}

	CurrentLaneIndex = laneIndex;

	if (moveLeft)
	{
		FVector offset = FVector(0, -LaneDistance, 0);
		AddActorWorldOffset(offset, true);

		CameraComponent->SetWorldLocation(FVector(
			CameraComponent->GetComponentLocation().X,
			0,
			CameraComponent->GetComponentLocation().Z));
	}

	if (moveRight)
	{
		FVector offset = FVector(0, LaneDistance, 0);
		AddActorWorldOffset(offset);


		CameraComponent->SetWorldLocation(FVector(
			CameraComponent->GetComponentLocation().X,
			0,
			CameraComponent->GetComponentLocation().Z));
	}

	UpdateCheckForPit();

	return true;
}

bool APlayerCharacter::CanPlayerOccupyLane(int laneIndex)
{
	//cancel if outside the 5 valid lanes
	if (laneIndex < 0 || laneIndex > 4)
	{
		return false;
	}

	return true;
}

float APlayerCharacter::GetCurrentRunSpeed()
{
	if (IsBoosting)
	{
		return BoostOverrideSpeed;
	}

	switch (CurrentSpeedState)
	{
	case EPlayerSpeedState::Slow:
		return SlowRunSpeed;
	case EPlayerSpeedState::Fast:
		return FastRunSpeed;
	default:
		return DefaultRunSpeed;
	}
}

void APlayerCharacter::UpdateJumpFromInput()
{
	if (!ContinueInput_Pressed)
	{
		if (JumpInput_Pressed)
		{
			bool jumpAllowed = false;

			switch (CurrentJumpState)
			{
			case EPlayerJumpState::Grounded:
				jumpAllowed = true;
				break;
			case EPlayerJumpState::Fall:
				jumpAllowed = TimeSinceLeftGround < MaxCoyoteTime;
				break;
			}

			if (TouchingBlockJumpSurface ||
				!HasJumpAvailable ||
				LastSurfaceWasBlockJump)
			{
				jumpAllowed = false;
			}


			if (jumpAllowed)
			{
				FVector Vel = GetCharacterMovement()->Velocity;
				Vel.Z = GetCharacterMovement()->JumpZVelocity; // use your config value
				GetCharacterMovement()->Velocity = Vel;
				
				bPressedJump = true;
				SetJumpState(EPlayerJumpState::Rise);
				JumpedThisFrame = true;
				LaneMovementBlocked = false;
				HasJumpAvailable = false;

				auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
				if (audioSystem)
				{
					audioSystem->Play(EAudioKey::Jump);
				}

				FActorSpawnParameters SpawnParams;
				FRotator defaultRotation = FRotator(0.0f, 90.0f, 0.0f);

				UPaperFlipbookComponent* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>();
				FVector vfxLoc = flipbookComp->GetComponentLocation();

				if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.JumpStartEffect, vfxLoc, defaultRotation, SpawnParams))
				{

				}
			}
		}

		if (JumpInput_Released)
		{
			if (!TouchingBlockJumpSurface &&
				CurrentJumpState != EPlayerJumpState::Grounded)
			{
				bPressedJump = false;
				SetJumpState(EPlayerJumpState::Fall);
			}
		}
	}
}

void APlayerCharacter::UpdateJumpState(float DeltaTime)
{
	bool apexToFall = false;

	if (TimeSinceJumpStateChange < JumpApexHangTime)
	{
		TimeSinceJumpStateChange += DeltaTime;
	}
	else
	{
		apexToFall = true;
	}

	switch (CurrentJumpState)
	{
	case EPlayerJumpState::Rise:
		if (!JumpedThisFrame)
		{
			if (GetVelocity().Z <= 0.01f)
			{
				SetJumpState(EPlayerJumpState::Apex);
			}
		}
		break;
	case EPlayerJumpState::Fall:
		if (IsTouchingSolidFloor())
		{
			SetAnimState(ECharacterAnimState::JumpLand);
			SetJumpState(EPlayerJumpState::Grounded);

			FActorSpawnParameters SpawnParams;
			FRotator defaultRotation = FRotator(0.0f, 90.0f, 0.0f);

			UPaperFlipbookComponent* flipbook = GetComponentByClass<UPaperFlipbookComponent>();
			FVector vfxLoc = flipbook->GetComponentLocation();

			if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.LandImpactEffect, vfxLoc, defaultRotation, SpawnParams))
			{

			}
		}
		break;
	case EPlayerJumpState::Apex:
		if (apexToFall)
		{
			SetJumpState(EPlayerJumpState::Fall);
		}
		break;
	case EPlayerJumpState::Grounded:
		if (GetVelocity().Z < 0)
		{
			SetJumpState(EPlayerJumpState::Fall);
		}
		break;
	}
}

void APlayerCharacter::UpdateCameraPos()
{
	//clamp camera Z pos
	FVector CameraClampZPos = FVector(CameraComponent->GetComponentLocation().X, 0.0f, CameraHeight);
	CameraComponent->SetWorldLocation(CameraClampZPos);
}


void APlayerCharacter::Shoot(EProjectileDirection direction, bool holdNotTap)
{
	if (!CanShootInDirection(direction, holdNotTap))
	{
		return;
	}

	auto* projectileSystem = GetGameInstance()->GetSubsystem<UGI_ProjectileSystem>();
	if (projectileSystem)
	{
		FProjectileRequestData requestData = FProjectileRequestData();

		FShootItem shootItem = FShootItem();

		shootItem.Direction = direction;
		shootItem.Type = EProjectileType::PlayerBullet;
		//shootItem.ShootPos = GetActorLocation();

		FVector shootPos = GetActorLocation();

		/*requestData.Direction = direction;
		requestData.ProjectileClass = ProjectileClass;*/

		//shootItem.ShootPos = GetActorLocation();

		float shootPosOffset = 100.0f;

		switch (direction)
		{
		case EProjectileDirection::Left:
			shootPos += FVector::LeftVector * shootPosOffset;
			break;
		case EProjectileDirection::Right:
			shootPos += FVector::RightVector * shootPosOffset;
			break;
		case EProjectileDirection::Up:
			shootPos += FVector::UpVector * shootPosOffset;
			break;
		case EProjectileDirection::Forward:
			shootPos += FVector::ForwardVector * shootPosOffset;
			break;
		}

		shootItem.ShootPos = shootPos;

		requestData.Items.Add(shootItem);

		if (projectileSystem->ProcessProjectileRequest(requestData))
		{
			FActorSpawnParameters SpawnParams;
			FRotator defaultRotation = FRotator(0.0f, 90.0f, 0.0f);

			FVector muzzleShotPos = shootItem.ShootPos;

			switch (direction)
			{
			case EProjectileDirection::Left:
				TimeSinceShoot_Left = 0;
				if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.MuzzleShot_Left, muzzleShotPos, defaultRotation, SpawnParams))
				{

				}
				break;
			case EProjectileDirection::Right:
				if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.MuzzleShot_Right, muzzleShotPos, defaultRotation, SpawnParams))
				{

				}
				TimeSinceShoot_Right = 0;
				break;
			case EProjectileDirection::Up:
				TimeSinceShoot_Up = 0;

				muzzleShotPos.Z += 65.0f;
				if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.MuzzleShot_Up, muzzleShotPos, defaultRotation, SpawnParams))
				{

				}
				break;
			case EProjectileDirection::Forward:
				TimeSinceShoot_Forward = 0;
				if (AOneShotAnim* impactAnim = GetWorld()->SpawnActor<AOneShotAnim>(ConfigData->VisualsConfig.MuzzleShot_Forward, muzzleShotPos, defaultRotation, SpawnParams))
				{

				}
				break;
			}
		}

		auto* audioSystem = GetGameInstance()->GetSubsystem<UGI_AudioSystem>();
		if (audioSystem)
		{
			audioSystem->Play(EAudioKey::PlayerShoot);
		}
	}
}

bool APlayerCharacter::CanShootInDirection(EProjectileDirection direction, bool holdNotTap)
{
	if (DelayPreventsShootInDirection(direction, holdNotTap))
	{
		return false;
	}

	if (ExceededProjCountForDirection(direction))
	{
		return false;
	}

	return true;
}

bool APlayerCharacter::DelayPreventsShootInDirection(EProjectileDirection direction, bool holdNotTap)
{
	if (holdNotTap)
	{
		

		switch (direction)
		{
		case EProjectileDirection::Forward:
			return TimeSinceShoot_Forward < ShootHoldInputDelay;
			break;
		case EProjectileDirection::Up:
			return TimeSinceShoot_Up < ShootHoldInputDelay;
			break;
		case EProjectileDirection::Left:
			return TimeSinceShoot_Left < ShootHoldInputDelay;
			break;
		case EProjectileDirection::Right:
			return TimeSinceShoot_Right < ShootHoldInputDelay;
			break;
		}
	}
	
	else
	{
		switch (direction)
		{
		case EProjectileDirection::Forward:
			return TimeSinceShoot_Forward < ShootTapInputDelay;
			break;
		case EProjectileDirection::Up:
			return TimeSinceShoot_Up < ShootTapInputDelay;
			break;
		case EProjectileDirection::Left:
			return TimeSinceShoot_Left < ShootTapInputDelay;
			break;
		case EProjectileDirection::Right:
			return TimeSinceShoot_Right < ShootTapInputDelay;
			break;
		}
	}

	return false;
}

bool APlayerCharacter::ExceededProjCountForDirection(EProjectileDirection direction)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerProjectile::StaticClass(), FoundActors);

	int activeProjectiles_Left = 0;
	int activeProjectiles_Right = 0;
	int activeProjectiles_Up = 0;
	int activeProjectiles_Forward = 0;

	for (AActor* Actor : FoundActors)
	{
		AProjectile* proj = Cast<AProjectile>(Actor);
		if (proj)
		{
			switch (proj->GetFiringDirection())
			{
			case EProjectileDirection::Left:
				activeProjectiles_Left++;
				break;
			case EProjectileDirection::Right:
				activeProjectiles_Right++;
				break;
			case EProjectileDirection::Up:
				activeProjectiles_Up++;
				break;
			case EProjectileDirection::Forward:
				activeProjectiles_Forward++;
				break;
			}
		}
	}

	switch (direction)
	{
	case EProjectileDirection::Left:
		return activeProjectiles_Left >= HoldShoot_MaxProjectiles;
		break;
	case EProjectileDirection::Right:
		return activeProjectiles_Right >= HoldShoot_MaxProjectiles;
		break;
	case EProjectileDirection::Up:
		return activeProjectiles_Up >= HoldShoot_MaxProjectiles;
		break;
	case EProjectileDirection::Forward:
		return activeProjectiles_Forward >= HoldShoot_MaxProjectiles;
		break;
	}

	return false;
}

void APlayerCharacter::UpdateShootValues(float DeltaTime)
{
	if (TimeSinceShoot_Left < ShootHoldInputDelay)
	{
		TimeSinceShoot_Left += DeltaTime;
	}
	if (TimeSinceShoot_Right < ShootHoldInputDelay)
	{
		TimeSinceShoot_Right += DeltaTime;
	}
	if (TimeSinceShoot_Up < ShootHoldInputDelay)
	{
		TimeSinceShoot_Up += DeltaTime;
	}
	if (TimeSinceShoot_Forward < ShootHoldInputDelay)
	{
		TimeSinceShoot_Forward += DeltaTime;
	}
}

void APlayerCharacter::UpdateShootFromInput()
{
	if (ShootLeftInput_Pressed)
	{
		Shoot(EProjectileDirection::Left, false);
		return;
	}
	if (ShootRightInput_Pressed)
	{
		Shoot(EProjectileDirection::Right, false);
		return;
	}
	if (ShootUpInput_Pressed)
	{
		Shoot(EProjectileDirection::Up, false);
		return;
	}
	if (ShootForwardInput_Pressed)
	{
		if (!BlockForwardShoot)
		{
			Shoot(EProjectileDirection::Forward, false);
			return;
		}
	}

	if (ShootLeftInput_Active)
	{
		Shoot(EProjectileDirection::Left, true);
		return;
	}
	if (ShootRightInput_Active)
	{
		Shoot(EProjectileDirection::Right, true);
		return;
	}
	if (ShootUpInput_Active)
	{
		Shoot(EProjectileDirection::Up, true);
		return;
	}
	if (ShootForwardInput_Active)
	{
		if (!BlockForwardShoot)
		{
			Shoot(EProjectileDirection::Forward, true);
			return;
		}
	}
}

void APlayerCharacter::UpdateCheckForPit()
{
	// Get start and end locations for the line trace
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 130.0f); // cast down

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // ignore self

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,   // trace channel
		TraceParams
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		float Distance = (Start - HitResult.ImpactPoint).Size();

		if (HitActor)
		{
			// Check if actor has a specific tag
			if (HitActor->ActorHasTag(FName("Pitfall")))
			{
				SetJumpState(EPlayerJumpState::Fall);
				LaneMovementBlocked = true;
			}
		}
	}
}

void APlayerCharacter::UpdateMercyInvincibility(float DeltaTime)
{
	if (!MercyInvincibleActive)
		return;

	MercyInvincibleTimeLeft -= DeltaTime;
	MercyInvincibleFlickerTimer += DeltaTime;

	if (MercyInvincibleTimeLeft <= 0.0f)
	{
		CancelMercyInvincibility();
		return;
	}

	if (MercyInvincibleFlickerTimer >= MercyInvincibleFlickerRate)
	{
		MercyInvincibleFlickerTimer = 0.0f;
		bMercyFlickerVisible = !bMercyFlickerVisible;

		if (UPaperFlipbookComponent* flipbookComp = GetComponentByClass<UPaperFlipbookComponent>())
		{
			flipbookComp->SetVisibility(bMercyFlickerVisible, true);
		}
	}
}

void APlayerCharacter::UpdateBoost(float DeltaTime)
{
	if (IsBoosting)
	{
		BoostTimeRemaining -= DeltaTime;
		if (BoostTimeRemaining <= 0)
		{
			CancelBoost();
		}
	}
}

void APlayerCharacter::UpdateCoyoteTime(float DeltaTime)
{
	if (CurrentJumpState == EPlayerJumpState::Fall)
	{
		if (TimeSinceLeftGround < MaxCoyoteTime)
		{
			TimeSinceLeftGround += DeltaTime;
		}
	}
	else
	{
		TimeSinceLeftGround = 0.0f;
	}
}

void APlayerCharacter::UpdateDropShadow()
{
	if (!ShadowPlaneMesh)
	{
		return;
	}

	if (CurrentJumpState == EPlayerJumpState::Grounded)
	{
		ShadowPlaneMesh->SetVisibility(false);
		return;
	}

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 500.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // don’t hit self

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility, // or custom channel
		Params
	);

	if (bHit && HitResult.GetActor()->ActorHasTag(FName("Floor")))
	{
		ShadowPlaneMesh->SetWorldLocation(HitResult.ImpactPoint + FVector(0, 0, 0.1f));

		FRotator ShadowRotation = HitResult.ImpactNormal.Rotation();
		ShadowRotation.Pitch -= 90.0f;
		ShadowPlaneMesh->SetWorldRotation(ShadowRotation);

		float HeightAboveFloor = (Start.Z - HitResult.ImpactPoint.Z);

		float MinScale = 1.0f;   // biggest size closest to ground
		float MaxScale = 0.3f;   // smallest size furthest from ground
		float MaxHeight = 300.0f; // height at which shadow is largest

		float ScaleFactor = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, MaxHeight),
			FVector2D(MinScale, MaxScale),
			HeightAboveFloor
		);

		ShadowPlaneMesh->SetWorldScale3D(FVector(ScaleFactor, ScaleFactor, 1.0f));

		ShadowPlaneMesh->SetVisibility(true);
	}
	else
	{
		ShadowPlaneMesh->SetVisibility(false);
	}
}

void APlayerCharacter::UpdateAnimation()
{
	if (JumpedThisFrame)
	{
		SetAnimState(ECharacterAnimState::JumpStart);
	}
}

void APlayerCharacter::UpdateDistanceTravelled()
{
	FVector CurrentPos = GetActorLocation();
	float Delta = CurrentPos.X - LastFramePos.X; // assuming X is forward

	// Only add positive movement (ignore knockbacks or backwards motion)
	if (Delta > 0)
	{
		DistanceTravelled += Delta;
	}

	LastFramePos = CurrentPos;

	OnDistanceSet.Broadcast();
}

void APlayerCharacter::OnFlipbookFinish()
{
	switch (CurrentAnimState)
	{
	case ECharacterAnimState::JumpStart:
		SetAnimState(ECharacterAnimState::JumpLoop);
		break;
	case ECharacterAnimState::JumpLand:
		SetAnimState(ECharacterAnimState::Grounded);
	}
}

bool APlayerCharacter::IsTouchingBlockJumpFloor()
{
	bool touchingBlockJump = false;

	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, ABlockJumpSurface::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor)
			continue;

		if (Actor->IsA(ABlockJumpSurface::StaticClass()))
		{
			touchingBlockJump = true;
		}
	}

	return touchingBlockJump;
}

bool APlayerCharacter::SolidBlockingLeftLane()
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.0f, -LaneDistance, 0.0f); // cast right

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic, // Use your floor collision channel
		FCollisionShape::MakeCapsule(
			GetCapsuleComponent()->GetScaledCapsuleRadius(),
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		),
		Params
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		float Distance = (Start - HitResult.ImpactPoint).Size();

		if (HitActor)
		{
			// Check if actor has a specific tag
			if (HitActor->ActorHasTag(FName("Floor")))
			{
				return true;
			}
		}
	}

	return false; return false;
}

bool APlayerCharacter::SolidBlockingRightLane()
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.0f, LaneDistance, 0.0f); // cast right

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic, // Use your floor collision channel
		FCollisionShape::MakeCapsule(
			GetCapsuleComponent()->GetScaledCapsuleRadius(),
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		),
		Params
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		float Distance = (Start - HitResult.ImpactPoint).Size();

		if (HitActor)
		{
			// Check if actor has a specific tag
			if (HitActor->ActorHasTag(FName("Floor")))
			{
				return true;
			}
		}
	}

	return false;
}

void APlayerCharacter::OnLevelRestart()
{
	ResetPlayer();
}

void APlayerCharacter::OnLevelExit()
{
	ResetPlayer();
}

float APlayerCharacter::GetDistanceTravelled_PureUnits()
{
	return DistanceTravelled;
}

float APlayerCharacter::GetDistanceTravelled_Meters()
{
	return DistanceTravelled / 100.f;
}

bool APlayerCharacter::IsTouchingSolidFloor()
{
	//if (!SolidProxy) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 5.f); // small distance below capsule

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Sweep a capsule downward
	bool bHit = World->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic, // Use your floor collision channel
		FCollisionShape::MakeCapsule(
			GetCapsuleComponent()->GetScaledCapsuleRadius(),
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		),
		Params
	);

	if (bHit)
	{
		// Check the tag on either the component or actor
		if (Hit.GetComponent() && Hit.GetComponent()->ComponentHasTag(TEXT("Floor")))
		{
			return true;
		}
		else if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("Floor")))
		{
			return true;
		}
	}

	return false;
}

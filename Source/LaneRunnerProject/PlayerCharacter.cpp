// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

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


	// assign sprite image
	if (SpriteComponent && PlayerSprite)
	{
		SpriteComponent->SetSprite(PlayerSprite);
	}

	// assign sprite transform
	FRotator DesiredRotation = FRotator(0.0f, 90.0f, 0.0f);
	SpriteComponent->SetWorldRotation(DesiredRotation);

	SpriteComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));


	//assign camera FOV
	CameraComponent->SetFieldOfView(CameraFOV);
	

	//set lane to middle
	SetLane(2);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLaneScroll();
	UpdateSpeedFromInput();

	UpdateLaneFromInput();

	UpdateJumpState(DeltaTime);
	UpdateJumpFromInput();

	UpdateCameraPos();

	ClearInputValues();
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

		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_SpeedUpStart);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SpeedUp);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SpeedUpCancel);

		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_SlowDownStart);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SlowDown);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SlowDownCancel);

		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_JumpStart);
		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_Jump);
		EnhancedInputComponent->BindAction(Input_JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_JumpCancel);
	}

}

void APlayerCharacter::BeginPlay_SetupFromConfig()
{
	if (ConfigData)
	{
		InputMap = ConfigData->InputConfig.InputMap;

		PlayerSprite = ConfigData->VisualsConfig.PlayerSprite;

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
		HoldShoot_MaxProjectiles = ConfigData->ShootConfig.HoldShoot_MaxProjectiles;

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up config data."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up config data EPIC FAIL."));
	}
}

void APlayerCharacter::Input_SetupFromConfig()
{
	if (ConfigData)
	{
		Input_LeftAction = ConfigData->InputConfig.Input_LeftAction;
		Input_RightAction = ConfigData->InputConfig.Input_RightAction;
		Input_SpeedUpAction = ConfigData->InputConfig.Input_SpeedUpAction;
		Input_SlowDownAction = ConfigData->InputConfig.Input_SlowDownAction;
		Input_ShootRightAction = ConfigData->InputConfig.Input_ShootRightAction;
		Input_ShootLeftAction = ConfigData->InputConfig.Input_ShootLeftAction;
		Input_ShootUpAction = ConfigData->InputConfig.Input_ShootUpAction;
		Input_ShootForwardAction = ConfigData->InputConfig.Input_ShootForwardAction;
		Input_JumpAction = ConfigData->InputConfig.Input_JumpAction;

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up input data."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("set up input data EPIC FAIL."));
	}

}

void APlayerCharacter::Input_LeftStart()
{
	LeftInput_Pressed = true;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("LEFT"));
}

void APlayerCharacter::Input_RightStart()
{
	RightInput_Pressed = true;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("RIGHT"));
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

void APlayerCharacter::Input_JumpStart(const FInputActionValue& Value)
{
	JumpInput_Pressed = true;
}

void APlayerCharacter::Input_Jump(const FInputActionValue& Value)
{
	JumpInput_Active = true;
}

void APlayerCharacter::Input_JumpCancel(const FInputActionValue& Value)
{
	JumpInput_Released = true;
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
}

void APlayerCharacter::UpdateLaneScroll()
{
	AddMovementInput(FVector(GetCurrentRunSpeed(), 0.0f, 0.0f));
}

void APlayerCharacter::UpdateLaneFromInput()
{
	if (LeftInput_Pressed)
	{
		MoveLane_Left();
	}

	if (RightInput_Pressed)
	{
		MoveLane_Right();
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
		newState = EPlayerSpeedState::Slow;
	}

	if (SpeedInput_Active && SlowInput_Active)
	{
		newState = EPlayerSpeedState::Default;
	}

	SetSpeedState(newState);


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

void APlayerCharacter::SetSpeedState(EPlayerSpeedState newState)
{
	CurrentSpeedState = newState;
}

void APlayerCharacter::SetJumpState(EPlayerJumpState newState)
{
	CurrentJumpState = newState;
	TimeSinceJumpStateChange = 0.0f;

	UCharacterMovementComponent* characterMovement = (UCharacterMovementComponent*)GetComponentByClass(UCharacterMovementComponent::StaticClass());

	switch (CurrentJumpState)
	{
	case EPlayerJumpState::Rise:
		characterMovement->GravityScale = JumpRiseGravity;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Rise."));
		break;
	case EPlayerJumpState::Apex:
		CancelVerticalSpeed();
		characterMovement->GravityScale = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Apex."));
		break;
	case EPlayerJumpState::Fall:
		characterMovement->GravityScale = JumpFallGravity;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Fall."));
		break;
	case EPlayerJumpState::Grounded:
		characterMovement->GravityScale = JumpRiseGravity;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Grounded."));
		break;
	}
}

void APlayerCharacter::CancelVerticalSpeed()
{
	GetRootComponent()->ComponentVelocity = FVector(GetRootComponent()->ComponentVelocity.X, GetRootComponent()->ComponentVelocity.Y, 0);
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
		AddActorWorldOffset(offset);

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
	if (JumpInput_Pressed)
	{
		if (CurrentJumpState == EPlayerJumpState::Grounded)
		{
			bPressedJump = true;
			SetJumpState(EPlayerJumpState::Rise);
			JumpedThisFrame = true;
		}
	}

	if (JumpInput_Released)
	{
		bPressedJump = false;
		SetJumpState(EPlayerJumpState::Fall);
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
		if (GetVelocity().Z == 0.0f)
		{
			SetJumpState(EPlayerJumpState::Grounded);
		}
		break;
	case EPlayerJumpState::Apex:
		if (apexToFall)
		{
			SetJumpState(EPlayerJumpState::Fall);
		}
		break;
	}
}

void APlayerCharacter::UpdateCameraPos()
{
	//clamp camera Z pos
	FVector CameraClampZPos = FVector(CameraComponent->GetComponentLocation().X, CameraComponent->GetComponentLocation().Y, CameraHeight);
	CameraComponent->SetWorldLocation(CameraClampZPos);
}

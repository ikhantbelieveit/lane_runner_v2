// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ScrollWithPlayerComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.h"
#include "PlayerProjectile.h"
#include "LevelSystem.h"
#include "ProjectileSystem.h"


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
	

	//Scroll Trigger Box
	ScrollTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ScrollTriggerBox"));
	ScrollTriggerBox->SetupAttachment(GetRootComponent());
	ScrollTriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ScrollTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	
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

	SpawnPos = GetActorLocation();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ALevelSystem* foundLevelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
	if (foundLevelSystem)
	{
		switch (foundLevelSystem->GetGameState())
		{
		case EGameState::Active:

			UpdateLaneScroll();
			UpdateSpeedFromInput();

			UpdateLaneFromInput();

			UpdateJumpState(DeltaTime);
			UpdateJumpFromInput();

			UpdateShootValues(DeltaTime);
			UpdateShootFromInput();

			UpdateCameraPos();

			ClearInputValues();
			break;
		case EGameState::Lose:
			UpdateCameraPos();

			ClearInputValues();
			break;
		}
	}
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
		EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_SpeedUpStart_Joystick);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SpeedUp_Joystick);
		EnhancedInputComponent->BindAction(Input_SpeedUpAction_Joystick, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SpeedUpCancel_Joystick);

		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Started, this, &APlayerCharacter::Input_SlowDownStart);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SlowDown);
		EnhancedInputComponent->BindAction(Input_SlowDownAction, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SlowDownCancel);
		EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Started, this, &APlayerCharacter::Input_SlowDownStart_Joystick);
		EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_SlowDown_Joystick);
		EnhancedInputComponent->BindAction(Input_SlowDownAction_Joystick, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SlowDownCancel_Joystick);

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

void APlayerCharacter::OnLevelResetFromLose()
{
	//set lane to middle
	SetLane(2);

	//Move back to start
	SetActorLocation(SpawnPos);
	UpdateCameraPos();
	
	ClearInputValues();	//maybe not needed? figured its handy
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
	FVector CameraClampZPos = FVector(CameraComponent->GetComponentLocation().X, 0.0f, CameraHeight);
	CameraComponent->SetWorldLocation(CameraClampZPos);
}


void APlayerCharacter::Shoot(EProjectileDirection direction, bool holdNotTap)
{
	if (!CanShootInDirection(direction, holdNotTap))
	{
		return;
	}

	AProjectileSystem* foundSystem = Cast<AProjectileSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AProjectileSystem::StaticClass()));
	if (foundSystem)
	{
		FProjectileRequestData requestData = FProjectileRequestData();
		requestData.Direction = direction;
		requestData.ProjectileClass = ProjectileClass;
		requestData.ShootPos = GetActorLocation();

		if (foundSystem->ShootPlayerProjectile(requestData))
		{
			switch (direction)
			{
			case EProjectileDirection::Left:
				TimeSinceShoot_Left = 0;
				break;
			case EProjectileDirection::Right:
				TimeSinceShoot_Right = 0;
				break;
			case EProjectileDirection::Up:
				TimeSinceShoot_Up = 0;
				break;
			case EProjectileDirection::Forward:
				TimeSinceShoot_Forward = 0;
				break;
			}
		}
	}
}

bool APlayerCharacter::CanShootInDirection(EProjectileDirection direction, bool holdNotTap)
{
	bool bypassDelay = !holdNotTap;

	if (!bypassDelay)
	{
		if (DelayPreventsShootInDirection(direction))
		{
			return false;
		}
	}

	if (holdNotTap)
	{
		if (ExceededProjCountForDirection(direction))
		{
			return false;
		}
	}

	return true;
}

bool APlayerCharacter::DelayPreventsShootInDirection(EProjectileDirection direction)
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
		Shoot(EProjectileDirection::Forward, false);
		return;
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
		Shoot(EProjectileDirection::Forward, true);
		return;
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
				GetCharacterMovement()->StopMovementImmediately();

				ALevelSystem* foundLevelSystem = Cast<ALevelSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSystem::StaticClass()));
				if (foundLevelSystem)
				{
					foundLevelSystem->OnPitfall();
				}
			}
		}
	}
}

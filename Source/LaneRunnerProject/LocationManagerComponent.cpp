#include "LocationManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "GI_LevelSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameInit.h"
#include "GroupOwnerComponent.h"

ULocationManagerComponent::ULocationManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULocationManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache player ref
	UWorld* World = GetWorld();
	if (World)
	{
		AActor* FoundPlayer = UGameplayStatics::GetActorOfClass(World, APlayerCharacter::StaticClass());
		PlayerRef = Cast<APlayerCharacter>(FoundPlayer);
		bHasPlayerRef = (PlayerRef != nullptr);
	}
	
	auto* levelSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_LevelSystem>();
	if (levelSystem)
	{
		levelSystem->CleanupBeforeReset.AddDynamic(this, &ULocationManagerComponent::Reset);
	}

	TargetActor = GetOwner();

	StartPos = TargetActor->GetActorLocation();

	Reset();
}

void ULocationManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!TargetActor) return;

	if (bScrollInterp)
	{
		FVector CurrentLoc = TargetActor->GetActorLocation();
		FVector TargetLoc = FVector(PlayerRef->GetActorLocation().X + ScrollWithXPos, CurrentLoc.Y, CurrentLoc.Z);
		TargetActor->SetActorLocation(FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, 10.0f));
		return;
	}

	FVector NewLoc = TargetActor->GetActorLocation();
	FRotator NewRot = TargetActor->GetActorRotation();

	if (bGravityEnabled)
	{
		UpdateGravity(DeltaTime);
	}

	UpdateAutoMove(DeltaTime);

	if (!bScrollEnabled)
		return;

	if (bScrollEnabled && bHasPlayerRef)
	{
		float NewX = PlayerRef->GetActorLocation().X + ScrollWithXPos;
		NewLoc.X = NewX;
	}

	TargetActor->SetActorLocationAndRotation(NewLoc, NewRot, false, nullptr, ETeleportType::TeleportPhysics);
}

void ULocationManagerComponent::UpdateScroll(FVector& InOutLocation, bool& bOutHasScroll)
{
	if (!bScrollEnabled || !bHasPlayerRef) return;

	float NewXPos = PlayerRef->GetActorLocation().X + ScrollWithXPos;
	InOutLocation.X = NewXPos; // override only X, keep Y/Z from path

	bOutHasScroll = true;
}

void ULocationManagerComponent::UpdateGravity(float DeltaTime)
{
	if (!bGravityEnabled) return;

	if (UProjectileMovementComponent* ProjMove = TargetActor->FindComponentByClass<UProjectileMovementComponent>())
	{
		bool bCheckForGround = false;

		// If velocity is near zero vertically, check for ground
		if (FMath::IsNearlyZero(ProjMove->Velocity.Z, KINDA_SMALL_NUMBER))
		{
			bCheckForGround = true;
		}

		if (bCheckForGround)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				FVector Start = TargetActor->GetActorLocation();
				FVector End = Start - FVector(0.f, 0.f, GroundCheckDistance);

				FCollisionObjectQueryParams ObjParams;
				ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

				FCollisionQueryParams Params;
				Params.AddIgnoredActor(GetOwner());

				FHitResult Hit;
				bool bOnGround = World->LineTraceSingleByObjectType(
					Hit, Start, End, ObjParams, Params);

				if (bOnGround)
				{
					ECollisionChannel ObjType = Hit.Component->GetCollisionObjectType();
					if (ObjType == ECC_GameTraceChannel1) // ignore collectible channel
					{
						bOnGround = false;
					}
				}

				if (!bOnGround)
				{
					// Not grounded so force gravity active
					SetGravityEnabled(true);
				}
			}
		}
	}
}

void ULocationManagerComponent::Reset()
{
	bScrollEnabled = bStartScrollActive;
	bAutoMoveEnabled = bStartAutoMoveEnabled;
	bDespawnOnAutoMoveEnd = false;

	SetAutoMoveSpeed(StartAutoMoveSpeed);
	SetAutoMoveDirection(StartAutoMoveDirection);

	StopAutoMove(false);

	TargetActor->SetActorLocation(StartPos);
}

void ULocationManagerComponent::SetGravityEnabled(bool bEnabled)
{
	if (UProjectileMovementComponent* ProjMove = TargetActor->FindComponentByClass<UProjectileMovementComponent>())
	{
		if (!ProjMove->UpdatedComponent)
		{
			ProjMove->SetUpdatedComponent(TargetActor->GetRootComponent());
		}

		ProjMove->Activate(bEnabled);

		if (bEnabled)
		{
			ProjMove->ProjectileGravityScale = 1.0f;
			ProjMove->Velocity = FVector(0.f, 0.f, -1.f);
		}
		else
		{
			ProjMove->ProjectileGravityScale = 0.0f;
		}
	}

	bGravityEnabled = bEnabled;
}

void ULocationManagerComponent::ApplyAutoMove()
{
	if (!bAutoMoveEnabled)
	{
		return;
	}

	if (UProjectileMovementComponent* ProjMove = TargetActor->FindComponentByClass<UProjectileMovementComponent>())
	{
		// Make sure it has a valid root to update
		if (!ProjMove->UpdatedComponent)
		{
			ProjMove->SetUpdatedComponent(TargetActor->GetRootComponent());
		}

		FVector Dir = FVector::ZeroVector;
		switch (CurrentAutoMoveDirection)
		{
		case EProjectileDirection::Forward:  Dir = FVector::ForwardVector;  break;
		case EProjectileDirection::Backward: Dir = -FVector::ForwardVector; break;
		case EProjectileDirection::Right:    Dir = FVector::RightVector;    break;
		case EProjectileDirection::Left:     Dir = -FVector::RightVector;   break;
		case EProjectileDirection::Up:       Dir = FVector::UpVector;       break;
		case EProjectileDirection::Down:     Dir = -FVector::UpVector;      break;
		default: break;
		}

		ProjMove->ProjectileGravityScale = 0.f; // pure linear motion unless you want gravity
		ProjMove->Velocity = Dir * CurrentAutoMoveSpeed;
		ProjMove->Activate(true);

		IsAutoMoving = true;

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("APPLY AUTO MOVE"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("AUTO MOVE SPEED: %s"), *ProjMove->Velocity.ToString()));
	}
}

void ULocationManagerComponent::SetAutoMoveSpeed(float newSpeed)
{
	CurrentAutoMoveSpeed = newSpeed;
}

void ULocationManagerComponent::SetAutoMoveDirection(EProjectileDirection newDirection)
{
	CurrentAutoMoveDirection = newDirection;
}

void ULocationManagerComponent::StartAutoMove(EProjectileDirection direction, float Speed, bool bUseStop, FVector stopCoords, bool despawnOnEnd)
{

	if (IsAutoMoving)
	{
		StopAutoMove(false);
	}

	bAutoMoveEnabled = true;
	CurrentAutoMoveDirection = direction;
	CurrentAutoMoveSpeed = Speed;

	// Optional stop coordinates
	bHasAutoMoveStop = bUseStop;
	if (bUseStop)
	{
		AutoMoveStopCoords = stopCoords;
	}

	bDespawnOnAutoMoveEnd = despawnOnEnd;

	// Apply immediately
	ApplyAutoMove();
}

void ULocationManagerComponent::StopAutoMove(bool clampToEnd)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("STOP AUTO MOVE"));

	UProjectileMovementComponent* projMoveComp = (UProjectileMovementComponent*)TargetActor->GetComponentByClass(UProjectileMovementComponent::StaticClass());
	if (projMoveComp)
	{
		projMoveComp->StopMovementImmediately();
	}

	bAutoMoveEnabled = false;
	bHasAutoMoveStop = false;

	IsAutoMoving = false;

	if (clampToEnd)
	{
		

		FVector currentLoc = TargetActor->GetActorLocation();

		switch (CurrentAutoMoveDirection)
		{
		case EProjectileDirection::Left:
		case EProjectileDirection::Right:
			TargetActor->SetActorLocation(FVector(currentLoc.X, AutoMoveStopCoords.Y, currentLoc.Z));
			break;
		case EProjectileDirection::Up:
		case EProjectileDirection::Down:
			TargetActor->SetActorLocation(FVector(currentLoc.X, currentLoc.Y, AutoMoveStopCoords.Z));
			break;
		case EProjectileDirection::Forward:
		case EProjectileDirection::Backward:
			TargetActor->SetActorLocation(FVector(AutoMoveStopCoords.X, currentLoc.Y, currentLoc.Z));
			break;
		}
	}

	if (bDespawnOnAutoMoveEnd)
	{
		if (UGroupOwnerComponent* ownerComp = TargetActor->GetComponentByClass<UGroupOwnerComponent>())
		{
			ownerComp->DespawnGroupMembers();
		}
	}

	OnAutoMoveEnd.Broadcast();
}

void ULocationManagerComponent::UpdateAutoMove(float DeltaTime)
{
	if (bHasAutoMoveStop)
	{
		FVector Loc = TargetActor->GetActorLocation();

		bool bStopReached = false;
		bool bStopX = false;
		bool bStopY = false;
		bool bStopZ = false;

		// Check X coordinate if set
		if ((CurrentAutoMoveDirection == EProjectileDirection::Forward && Loc.X >= AutoMoveStopCoords.X) ||
			(CurrentAutoMoveDirection == EProjectileDirection::Backward && Loc.X <= AutoMoveStopCoords.X))
		{
			bStopReached = true;
			bStopX = true;
		}

		// Check Y coordinate if set
		if ((CurrentAutoMoveDirection == EProjectileDirection::Right && Loc.Y >= AutoMoveStopCoords.Y) ||
			(CurrentAutoMoveDirection == EProjectileDirection::Left && Loc.Y <= AutoMoveStopCoords.Y))
		{
			bStopReached = true;
			bStopY = true;
		}

		if ((CurrentAutoMoveDirection == EProjectileDirection::Up && Loc.Z >= AutoMoveStopCoords.Z) ||
			(CurrentAutoMoveDirection == EProjectileDirection::Down && Loc.Z <= AutoMoveStopCoords.Z))
		{
			bStopReached = true;
			bStopZ = true;
		}

		if (bStopReached)
		{
			StopAutoMove(true);
			return;
		}
	}
}

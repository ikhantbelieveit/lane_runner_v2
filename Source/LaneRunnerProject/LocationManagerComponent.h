#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "EProjectileDirection.h"
#include "LocationManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EPathFollowMode : uint8
{
	Clamp    UMETA(DisplayName = "Clamp"),
	Loop     UMETA(DisplayName = "Loop"),
	PingPong UMETA(DisplayName = "Ping-Pong")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLocationEvent);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LANERUNNERPROJECT_API ULocationManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULocationManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** === Scroll With Player Settings === */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll")
	bool bScrollEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll")
	float ScrollWithXPos = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll")
	bool bStartScrollActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll")
	bool bScrollInterp = false;

	/** === Path Follower Settings === */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	AActor* SplineActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	AActor* InitialSplineActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float DefaultSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bFollowEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bStartFollowEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bRotateToMatchPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	EPathFollowMode FollowMode = EPathFollowMode::Clamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bGravityEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float GroundCheckDistance = 5.0f; // how far below actor to trace

	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void SetGravityEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Gravity")
	bool IsGravityEnabled() const { return bGravityEnabled; }

	/** === Runtime State === */
	UFUNCTION(BlueprintCallable)
	void Reset();

	void ResetPath();
	void SetPathSpeed(float NewSpeed);
	void SetSpline(USplineComponent* NewSpline);
	void ClearSpline();

	float GetCurrentSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoMove")
	bool bAutoMoveEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoMove")
	bool bStartAutoMoveEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoMove")
	EProjectileDirection StartAutoMoveDirection = EProjectileDirection::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoMove")
	float StartAutoMoveSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, Category = "AutoMove")
	bool bHasAutoMoveStop = false;

	UPROPERTY(VisibleAnywhere, Category = "AutoMove")
	FVector AutoMoveStopCoords = FVector::ZeroVector;

	// Call to apply auto-move settings
	UFUNCTION(BlueprintCallable, Category = "AutoMove")
	void ApplyAutoMove();

	void SetAutoMoveSpeed(float newSpeed);
	void SetAutoMoveDirection(EProjectileDirection newDirection);
	void StartAutoMove(EProjectileDirection direction, float Speed, bool bUseStop, FVector stopCoords, bool despawnOnEnd);
	void StopAutoMove(bool clampToEnd);

	void UpdateAutoMove(float DeltaTime);

	UPROPERTY(BlueprintAssignable)
	FLocationEvent OnAutoMoveEnd;

private:
	// Scroll
	class APlayerCharacter* PlayerRef = nullptr;
	bool bHasPlayerRef = false;
	FVector StartPos;

	// Path follow
	USplineComponent* CurrentSplineComp = nullptr;
	float CurrentPathSpeed = 0.f;
	float DistanceAlongSpline = 0.f;
	int Direction = 1;

	//auto move
	EProjectileDirection CurrentAutoMoveDirection;
	float CurrentAutoMoveSpeed;
	bool IsAutoMoving = false;

	// Helpers
	void UpdatePath(float DeltaTime, FVector& OutLocation, FRotator& OutRotation, bool& bOutHasPath);
	void UpdateScroll(FVector& InOutLocation, bool& bOutHasScroll);
	void UpdateGravity(float DeltaTime);

	AActor* TargetActor;

	bool bDespawnOnAutoMoveEnd = false;
};
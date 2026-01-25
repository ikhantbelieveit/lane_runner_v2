#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "EProjectileDirection.h"
#include "LocationManagerComponent.generated.h"

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

	UPROPERTY(BlueprintAssignable)
	FLocationEvent OnMoveToDespawnStart;

private:
	// Scroll
	class APlayerCharacter* PlayerRef = nullptr;
	bool bHasPlayerRef = false;
	FVector StartPos;

	//auto move
	EProjectileDirection CurrentAutoMoveDirection;
	float CurrentAutoMoveSpeed;
	bool IsAutoMoving = false;

	// Helpers
	void UpdateScroll(FVector& InOutLocation, bool& bOutHasScroll);
	void UpdateGravity(float DeltaTime);

	AActor* TargetActor;

	bool bDespawnOnAutoMoveEnd = false;
};
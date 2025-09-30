#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "LocationManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EPathFollowMode : uint8
{
	Clamp    UMETA(DisplayName = "Clamp"),
	Loop     UMETA(DisplayName = "Loop"),
	PingPong UMETA(DisplayName = "Ping-Pong")
};

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

	/** === Path Follower Settings === */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	AActor* SplineActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float DefaultSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bFollowEnabled = false;

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
	void Reset();

	void ResetPath();
	void SetSpeed(float NewSpeed);
	void SetSpline(USplineComponent* NewSpline);

private:
	// Scroll
	class APlayerCharacter* PlayerRef = nullptr;
	bool bHasPlayerRef = false;
	FVector StartPos;

	// Path follow
	USplineComponent* CurrentSplineComp = nullptr;
	float CurrentSpeed = 0.f;
	float DistanceAlongSpline = 0.f;
	int Direction = 1;

	// Helpers
	void UpdatePath(float DeltaTime, FVector& OutLocation, FRotator& OutRotation, bool& bOutHasPath);
	void UpdateScroll(FVector& InOutLocation, bool& bOutHasScroll);
	void UpdateGravity(float DeltaTime);
};
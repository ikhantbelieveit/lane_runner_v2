// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "ProjectileRequestData.h"
#include "GroupMember.h"
#include "LocationManagerComponent.h"
#include "ChunkInitializable.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyDetectBehaviour : uint8
{
	None UMETA(DisplayName = "None"),
	StraightAdvance UMETA(DisplayName = "Straight Advance"),
	Shoot_OneOff UMETA(DisplayName = "Shoot (One-Off)")
};

UCLASS()
class LANERUNNERPROJECT_API ABaseEnemy : public AActor, public IGroupMemberInterface, public IChunkInitializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPaperSpriteComponent* AlertVFX;
	UPaperSpriteComponent* MainVisuals;

	UPaperFlipbookComponent* MainVisualsFlipbook;

	bool IsAlive;

	bool PerformedOneOffShoot;
	bool PerformedAdvance;

	bool HasPerformedDetectAction();

	TMap<FName, USceneComponent*> ProjectileOrigins;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyDetectBehaviour DetectBehaviour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdvanceSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProjectileDirection AdvanceDirection = EProjectileDirection::Backward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScrollOnPlayerDetect;

	UFUNCTION()
	void OnDetectPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsEnemyAlive();

	UFUNCTION(BlueprintCallable)
	void OnKilled();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* IdleSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* AlertSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FProjectileRequestData> ProjectileDataMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UPaperFlipbook*> FlipbookMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* DefaultFlipbook;

	UFUNCTION()
	void OnLevelReset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowAlertVFX = true;

	UFUNCTION()
	void SetAnim(FString animName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Group")
	TObjectPtr<ULocationManagerComponent> GroupManagerRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Group")
	TObjectPtr<AActor> GroupActorRef;

public:
	// IGroupMemberInterface overrides
	virtual void OnAddedToGroup_Implementation(AActor* InGroupActor, ULocationManagerComponent* Manager) override;
	virtual void OnRemovedFromGroup_Implementation() override;
	virtual ULocationManagerComponent* GetGroupManager_Implementation() const override { return GroupManagerRef; }
	virtual AActor* GetGroupActor_Implementation() const override { return GroupActorRef; }
};

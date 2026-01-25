// Fill out your copyright notice in the Description page of Project Settings.


#include "WarningSign.h"
#include "GI_WarningSignSystem.h"
#include "SpawnComponent.h"


// Sets default values
AWarningSign::AWarningSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWarningSign::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWarningSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWarningSign::InitialiseSign(FWarningSignData sign)
{
	EnemyRef = sign.EnemyRef;
	GroupRef = sign.GroupRef;

	EnemyRef->OnEnemyKilled.AddDynamic(this, &AWarningSign::ResetSign);
	ULocationManagerComponent* locationComp = GroupRef->FindComponentByClass<ULocationManagerComponent>();
	if (locationComp)
	{
		LocationManagerRef = locationComp;
		LocationManagerRef->OnAutoMoveEnd.AddDynamic(this, &AWarningSign::ResetSign);
	}
}

void AWarningSign::ResetSign()
{
	if (EnemyRef.IsValid())
	{
		EnemyRef->OnEnemyKilled.RemoveDynamic(this, &AWarningSign::ResetSign);
	}
	
	if (LocationManagerRef.IsValid())
	{
		LocationManagerRef->OnAutoMoveEnd.RemoveDynamic(this, &AWarningSign::ResetSign);
	}

	EnemyRef = nullptr;
	LocationManagerRef = nullptr;
	GroupRef = nullptr;

	ClearSign();

	UGI_WarningSignSystem* signSystem = GetWorld()->GetGameInstance()->GetSubsystem<UGI_WarningSignSystem>();
	if (signSystem)
	{
		signSystem->ReturnSignToPool(this);
	}
}

void AWarningSign::ClearSign()
{
	if (USpawnComponent* SpawnComp = FindComponentByClass<USpawnComponent>())
	{
		SpawnComp->Despawn();
	}
}


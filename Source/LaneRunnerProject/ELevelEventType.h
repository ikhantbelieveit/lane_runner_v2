// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ELevelEventType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELevelEventType : uint8
{
	None    UMETA(DisplayName = "None"),
	TogglePlayerScroll    UMETA(DisplayName = "Toggle Player Scroll"),
	SetObjectPath    UMETA(DisplayName = "Set Object Path"),
	SetObjectSpeed    UMETA(DisplayName = "Set Object Speed"),
	TogglePlatform   UMETA(DisplayName = "Toggle Platform"),
	TogglePathFollow UMETA(DisplayName = "Toggle Path Follow"),
	InvertSpeed UMETA(DisplayName = "Invert Path Speed"),
	SetAutoMoveSpeed UMETA(DisplayName = "Set Auto Move Speed"),
	SetAutoMoveDirection UMETA(DisplayName = "Set Auto Move Direction"),
	StartAutoMove UMETA(DisplayName = "Start Auto Move"),
	StopAutoMove UMETA(DisplayName = "Stop Auto Move"),
	SpawnObjects UMETA(DisplayName = "Spawn Objects"),
	DespawnObjects UMETA(DisplayName = "Despawn Objects"),
	SpawnObjectGroup UMETA(DisplayName = "Spawn Object Group"),
	DespawnObjectGroup UMETA(DisplayName = "Despawn Object Group"),
	StartAutoMoveGroup UMETA(DisplayName = "Start Auto Move Group"),
	StopAutoMoveGroup UMETA(DisplayName = "Stop Auto Move Group"),
	MoveToDespawn UMETA(DisplayName = "Move to Despawn"),
	SetAlert UMETA(DisplayName = "Set Alert"),
	MoveToAlert UMETA(DisplayName = "Move to Alert"),
	SetIdle UMETA(DisplayName = "Set Idle"),
	StartDynamicMovement UMETA(DisplayName = "Start Dynamic Movement")
};
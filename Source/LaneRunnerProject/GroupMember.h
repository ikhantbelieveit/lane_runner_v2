// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GroupMember.generated.h"

class ULocationManagerComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UGroupMemberInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any actor that can belong to a group managed by a LocationManagerComponent.
 */
class LANERUNNERPROJECT_API IGroupMemberInterface
{
	GENERATED_BODY()

public:

	/** Called when the actor joins a group */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAddedToGroup(AActor* InGroupActor, ULocationManagerComponent* Manager);

	/** Called when the actor leaves the group */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRemovedFromGroup();

	/** Returns the cached manager reference (if any) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ULocationManagerComponent* GetGroupManager() const;

	/** Returns the current parent/group actor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetGroupActor() const;
};
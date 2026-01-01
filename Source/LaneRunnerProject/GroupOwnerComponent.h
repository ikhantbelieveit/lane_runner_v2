// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnComponent.h"
#include "GroupOwnerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LANERUNNERPROJECT_API UGroupOwnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGroupOwnerComponent();

	virtual void BeginPlay() override;

protected:
	/** All actors this group owns */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Group")
	TArray<AActor*> GroupMembers;

public:
	/** Add a new member to this group */
	UFUNCTION(BlueprintCallable, Category = "Group")
	void AddGroupMember(AActor* NewMember);

	/** Remove a member from this group */
	UFUNCTION(BlueprintCallable, Category = "Group")
	void RemoveGroupMember(AActor* Member);

	/** Get valid (non-null) member actors */
	UFUNCTION(BlueprintCallable, Category = "Group")
	TArray<AActor*> GetGroupMembers() const;

	/** Spawn all members (simple version) */
	UFUNCTION(BlueprintCallable, Category = "Group")
	void SpawnGroupMembers();

	/** Spawn all members (advanced parameters) */
	UFUNCTION(BlueprintCallable, Category = "Group")
	void SpawnGroupMembersAdvanced(bool bDrop, bool bScrollInstant, bool bScrollOnReach);

	/** Despawn all members */
	UFUNCTION(BlueprintCallable, Category = "Group")
	void DespawnGroupMembers();

	UFUNCTION(BlueprintCallable, Category = "Group")
	void AlertAllGroupMembers();

	UFUNCTION(BlueprintCallable, Category = "Group")
	void UnAlertAllGroupMembers();

		
};

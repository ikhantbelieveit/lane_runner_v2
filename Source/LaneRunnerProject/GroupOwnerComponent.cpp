// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupOwnerComponent.h"

UGroupOwnerComponent::UGroupOwnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGroupOwnerComponent::BeginPlay()
{
	TArray<AActor*> childActors;

	GetOwner()->GetAllChildActors(childActors, true);

	for (AActor* child : childActors)
	{
		if (!child) continue;

		AddGroupMember(child);
	}
}

void UGroupOwnerComponent::AddGroupMember(AActor* NewMember)
{
	if (NewMember)
	{
		GroupMembers.AddUnique(NewMember);
	}
}

void UGroupOwnerComponent::RemoveGroupMember(AActor* Member)
{
	GroupMembers.Remove(Member);
}

TArray<AActor*> UGroupOwnerComponent::GetGroupMembers() const
{
	return GroupMembers;
}

void UGroupOwnerComponent::SpawnGroupMembers()
{
	SpawnGroupMembersAdvanced(false, false, false);
}

void UGroupOwnerComponent::SpawnGroupMembersAdvanced(bool bDrop, bool bScrollInstant, bool bScrollOnReach)
{
	for (const TWeakObjectPtr<AActor>& WeakMember : GroupMembers)
	{
		if (AActor* Member = WeakMember.Get())
		{
			if (USpawnComponent* SpawnComp = Member->FindComponentByClass<USpawnComponent>())
			{
				SpawnComp->Spawn(bDrop, bScrollInstant, bScrollOnReach);
			}
		}
	}
}

void UGroupOwnerComponent::DespawnGroupMembers()
{
	for (const TWeakObjectPtr<AActor>& WeakMember : GroupMembers)
	{
		if (AActor* Member = WeakMember.Get())
		{
			if (USpawnComponent* SpawnComp = Member->FindComponentByClass<USpawnComponent>())
			{
				SpawnComp->Despawn();
			}
		}
	}
}
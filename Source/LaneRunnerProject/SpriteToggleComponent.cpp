// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperSpriteComponent.h"
#include "SpriteToggleComponent.h"

// Sets default values for this component's properties
USpriteToggleComponent::USpriteToggleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpriteToggleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USpriteToggleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USpriteToggleComponent::SetSpriteEnabled(FString spriteTag)
{
	FName name = FName(spriteTag);

	TArray<UPaperSpriteComponent*> SpriteComponents;
	GetOwner()->GetComponents<UPaperSpriteComponent>(SpriteComponents);

	for (UPaperSpriteComponent* SpriteComp : SpriteComponents)
	{
		if (!SpriteComp) continue;

		// Check if this component has the target tag
		if (SpriteComp->ComponentHasTag(name))
		{
			SpriteComp->SetVisibility(true);
		}
		else
		{
			SpriteComp->SetVisibility(false);
		}
	}
}

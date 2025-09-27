#include "DamageFlashComponent.h"
#include "PaperSpriteComponent.h"
#include "TimerManager.h"

UDamageFlashComponent::UDamageFlashComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDamageFlashComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get the sprite's dynamic material
    UPaperSpriteComponent* SpriteComp = GetOwner()->FindComponentByClass<UPaperSpriteComponent>();
    if (SpriteComp && SpriteComp->GetMaterial(0))
    {
        DynamicMaterial = SpriteComp->CreateDynamicMaterialInstance(0);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetScalarParameterValue("FlashAmount", 0.0f);
            DynamicMaterial->SetVectorParameterValue("FlashColor", FlashColor);
        }
    }
}

void UDamageFlashComponent::TriggerFlash()
{
    if (!DynamicMaterial) return;

    DynamicMaterial->SetVectorParameterValue("FlashColor", FlashColor);
    DynamicMaterial->SetScalarParameterValue("FlashAmount", 1.0f);

    // Fade back after duration
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            FlashTimerHandle,
            [this]()
            {
                if (DynamicMaterial)
                    DynamicMaterial->SetScalarParameterValue("FlashAmount", 0.0f);
            },
            FlashDuration,
            false
        );
    }
}
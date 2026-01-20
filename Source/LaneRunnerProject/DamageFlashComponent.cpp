#include "DamageFlashComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"

UDamageFlashComponent::UDamageFlashComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDamageFlashComponent::BeginPlay()
{
    Super::BeginPlay();

    Initialise();
}

void UDamageFlashComponent::Initialise()
{
    UPaperSpriteComponent* foundMainVisualsSprite = nullptr;

    UPaperFlipbookComponent* foundMainVisualsFlipbook = nullptr;

    TArray<UPaperSpriteComponent*> spriteComps;
    TArray<UPaperFlipbookComponent*> flipbookComps;

    GetOwner()->GetComponents<UPaperSpriteComponent>(spriteComps);
    GetOwner()->GetComponents<UPaperFlipbookComponent>(flipbookComps);

    for (UPaperSpriteComponent* sprite : spriteComps)
    {
        if (!sprite)
        {
            continue;
        }

        if (sprite->ComponentHasTag("MainVisuals"))
        {
            foundMainVisualsSprite = sprite;
        }
    }

    for (UPaperFlipbookComponent* flipbook : flipbookComps)
    {
        if (!flipbook)
        {
            continue;
        }

        if (flipbook->ComponentHasTag("MainVisuals"))
        {
            foundMainVisualsFlipbook = flipbook;
        }
    }

    UActorComponent* VisualComp = nullptr;

    if (foundMainVisualsFlipbook)
    {
        VisualComp = foundMainVisualsFlipbook;
    }

    else
    {
        if (foundMainVisualsSprite)
        {
            VisualComp = foundMainVisualsSprite;
        }
    }

    if (VisualComp)
    {
        SetupMaterial(VisualComp);
    }
}

void UDamageFlashComponent::SetupMaterial(UActorComponent* visualsComp)
{
    UMeshComponent* MeshComp = Cast<UMeshComponent>(visualsComp);
    if (MeshComp && MeshComp->GetMaterial(0))
    {
        DynamicMaterial = MeshComp->CreateDynamicMaterialInstance(0);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetScalarParameterValue("FlashAmount", 0.0f);
            //DynamicMaterial->SetVectorParameterValue("FlashColor", FlashColor);
            MeshComp->SetMaterial(0, DynamicMaterial);
        }
    }
}

void UDamageFlashComponent::TriggerFlash()
{
    if (!DynamicMaterial)
    {
        return;
    }

    //DynamicMaterial->SetVectorParameterValue("FlashColor", FlashColor);
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
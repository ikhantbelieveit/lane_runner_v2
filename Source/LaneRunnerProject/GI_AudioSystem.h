// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GI_AudioSystem.generated.h"

UENUM(BlueprintType)
enum class EAudioChannel : uint8
{
    Music,
    SFX,
    UI
};

UENUM(BlueprintType)
enum class EAudioKey : uint8
{
    None        UMETA(DisplayName = "None"),
    Gunfire   UMETA(DisplayName = "Gunfire"),
    LaneSwitch    UMETA(DisplayName = "LaneSwitch"),
    MenuClick   UMETA(DisplayName = "Menu Click"),
    BackgroundMusic  UMETA(DisplayName = "Background Music"),
    Jump UMETA(DisplayName = "Jump"),
    Pitfall UMETA(DisplayName = "Pitfall"),
    TakeDamage UMETA(DisplayName = "Take Damage"),
    GameOver UMETA(DisplayName = "Game Over"),
    CoinGet UMETA(DisplayName = "Coin Get"),
};

USTRUCT(BlueprintType)
struct FAudioEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EAudioKey Key;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Sound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundConcurrency* Concurrency = nullptr;
};

UCLASS(BlueprintType)
class LANERUNNERPROJECT_API UAudioConfigData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FAudioEntry> Sounds;
};


UCLASS()
class LANERUNNERPROJECT_API UGI_AudioSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Subsystem lifecycle
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Public API
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void Play(EAudioKey Key);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayMusic(EAudioKey Key);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopMusic(float FadeOutTime = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetChannelVolume(EAudioChannel Channel, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetChannelVolume(EAudioChannel Channel) const;

    bool InitialiseFromConfig();

    bool HasInitialisedFromConfig;

private:
    // Helpers
    UAudioComponent* CreateNewAudioComponent(USoundBase* Sound);
    USoundClass* ResolveClass(EAudioChannel Channel) const;
        

    TMap<EAudioKey, FAudioEntry> AudioMap;

    // Music playback
    UPROPERTY()
    UAudioComponent* MusicComponent = nullptr;

    // Sound categories
    UPROPERTY(EditAnywhere, Category = "Classes")
    USoundClass* MusicClass;

    UPROPERTY(EditAnywhere, Category = "Classes")
    USoundClass* SFXClass;

    UPROPERTY(EditAnywhere, Category = "Classes")
    USoundClass* UIClass;

    UPROPERTY(EditAnywhere, Category = "Mix")
    USoundMix* MasterMix;

    FTimerHandle TickHandle;

    void TickSubsystem(float DeltaTime);
};
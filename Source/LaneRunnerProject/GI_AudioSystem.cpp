#include "GI_AudioSystem.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "Components/AudioComponent.h"

void UGI_AudioSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TickHandle,
            [this]()
            {
                TickSubsystem(0.016f); // Approx 60 FPS
            },
            0.016f,
            true
        );
    }
}

void UGI_AudioSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }
    Super::Deinitialize();
}

void UGI_AudioSystem::TickSubsystem(float DeltaTime)
{
    if (!HasInitialisedFromConfig)
    {
        if (InitialiseFromConfig())
        {
            HasInitialisedFromConfig = true;
        }
    }
}

bool UGI_AudioSystem::InitialiseFromConfig()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (GI)
    {
        UAudioConfigData* configData = GI->ConfigData->AudioConfig;

        if (configData)
        {
            AudioMap.Empty();

            for (const FAudioEntry& Entry : configData->Sounds)
            {
                if (Entry.Key != EAudioKey::None)
                {
                    AudioMap.Add(Entry.Key, Entry);
                }
            }
            return true;
        }
    }

    return false;
}

void UGI_AudioSystem::Play(EAudioKey Key)
{
    if (FAudioEntry* Entry = AudioMap.Find(Key))
    {
        if (Entry->Sound)
        {
            UGameplayStatics::PlaySound2D(
                GetWorld(),
                Entry->Sound,
                1.0f,
                1.0f,
                0.0f,
                Entry->Concurrency
            );
        }
    }
}

void UGI_AudioSystem::PlayMusic(EAudioKey Key)
{
    if (FAudioEntry* Entry = AudioMap.Find(Key))
    {
        if (Entry->Sound)
        {
            if (!MusicComponent)
            {
                MusicComponent = CreateNewAudioComponent(Entry->Sound);
            }

            MusicComponent->Stop();
            MusicComponent->SetSound(Entry->Sound);
            MusicComponent->Play();

            if (MusicClass)
            {
                MusicComponent->SoundClassOverride = MusicClass;
            }
        }
    }
}

void UGI_AudioSystem::StopMusic(float FadeOutTime)
{
    if (MusicComponent && MusicComponent->IsPlaying())
    {
        MusicComponent->FadeOut(FadeOutTime, 0.0f);
    }
}

void UGI_AudioSystem::SetChannelVolume(EAudioChannel Channel, float Volume)
{
    if (USoundClass* TargetClass = ResolveClass(Channel))
    {
        TargetClass->Properties.Volume = Volume;

        if (MasterMix)
        {
            UGameplayStatics::SetSoundMixClassOverride(
                GetWorld(),
                MasterMix,
                TargetClass,
                Volume,
                1.0f,
                0.0f,
                true
            );
            UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterMix);
        }
    }
}

float UGI_AudioSystem::GetChannelVolume(EAudioChannel Channel) const
{
    if (USoundClass* TargetClass = ResolveClass(Channel))
    {
        return TargetClass->Properties.Volume;
    }
    return 1.0f;
}

UAudioComponent* UGI_AudioSystem::CreateNewAudioComponent(USoundBase* Sound)
{
    UAudioComponent* AudioComp = NewObject<UAudioComponent>(this);
    AudioComp->RegisterComponentWithWorld(GetWorld());
    AudioComp->SetSound(Sound);
    AudioComp->bAutoDestroy = false;
    return AudioComp;
}

USoundClass* UGI_AudioSystem::ResolveClass(EAudioChannel Channel) const
{
    switch (Channel)
    {
    case EAudioChannel::Music: return MusicClass;
    case EAudioChannel::SFX:   return SFXClass;
    case EAudioChannel::UI:    return UIClass;
    default: return nullptr;
    }
}
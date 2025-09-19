// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

void UMyGameInstance::Init()
{
    Super::Init();
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("using my game instance"));
}

void UMyGameInstance::Shutdown()
{
    Super::Shutdown();

}
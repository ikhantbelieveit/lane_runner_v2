// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullseye.h"
#include "LocationManagerComponent.h"
#include "SpawnComponent.h"

// Sets default values
ABullseye::ABullseye()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABullseye::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullseye::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullseye::InitializeFromChunkData_Implementation(const FChunkSpawnEntry& Entry)
{
    if (Entry.Metadata.IsEmpty()) return;


    // Parse JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Entry.Metadata);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        bool bScrollEnabled = false;
        if (JsonObject->HasField(TEXT("ScrollEnabled")))
        {
            bScrollEnabled = JsonObject->GetBoolField(TEXT("ScrollEnabled"));
            
        }


        float ScrollXPos = 0.0f;
        if (JsonObject->HasField(TEXT("ScrollXPos")))
        {
            ScrollXPos = JsonObject->GetNumberField(TEXT("ScrollXPos"));
        }

        bool bStartSpawned = false;
        if (JsonObject->HasField(TEXT("StartSpawned")))
        {
            bStartSpawned = JsonObject->GetBoolField(TEXT("StartSpawned"));
        }

        if (ULocationManagerComponent* locManager = FindComponentByClass<ULocationManagerComponent>())
        {
            locManager->bStartScrollActive = bScrollEnabled;
            locManager->bScrollEnabled = bScrollEnabled;
            locManager->ScrollWithXPos = ScrollXPos;
        }

        if (USpawnComponent* spawnComp = FindComponentByClass<USpawnComponent>())
        {
            spawnComp->ResetAsSpawned = bStartSpawned;
        }
    }
}


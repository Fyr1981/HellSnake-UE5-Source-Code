// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Placeables/HSSpawningGeometryCollection.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSSpawningGeometryCollection, All, All)

AHSSpawningGeometryCollection::AHSSpawningGeometryCollection()
{
    if (HasAuthority())
    {
        GeometryCollectionComponent->bNotifyBreaks = true;
    }
}

void AHSSpawningGeometryCollection::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        GeometryCollectionComponent->OnChaosBreakEvent.AddDynamic(this, &AHSSpawningGeometryCollection::Multicast_SpawnActorInside);
    }
}

void AHSSpawningGeometryCollection::Multicast_SpawnActorInside_Implementation(const FChaosBreakEvent& BreakEvent)
{
    if (!SpawningClass || ActorsSpawned >= SpawningMaxCount) { return; }
    //    UE_LOG(LogHSSpawningGeometryCollection, Error, TEXT("Spawn actor inside!"));
    GetWorld()->SpawnActor<AActor>(SpawningClass, FTransform(BreakEvent.Location));
    ActorsSpawned++;
    GeometryCollectionComponent->CanCharacterStepUpOn = ECB_Yes;
}

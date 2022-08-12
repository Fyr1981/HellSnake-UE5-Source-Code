// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Placeables/HSRandomSpawner.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRandomSpawner, All, All)

AHSRandomSpawner::AHSRandomSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetRootComponent(BoxComponent);
}

void AHSRandomSpawner::BeginPlay()
{
    Super::BeginPlay();
    RandomSpawn();
}

void AHSRandomSpawner::RandomSpawn()
{
    if (!bInfiniteSpawns && SpawnsHappened >= MaxCountOfSpawns) { Destroy(); }

    const int32 ActorsToSpawn = SpawnsHappened ? ActorsPerSpawn : ActorsOnFirstSpawn;
    for (int32 i = 0; i < ActorsToSpawn; i++)
    {
        const auto NewActor = GetWorld()->SpawnActor<AActor>(SpawningActorClass, FTransform(GetRandomLocation()));
        NewActor->OnDestroyed.AddDynamic(this, &AHSRandomSpawner::OnActorDestroyed);
    }

    SpawnsHappened ++;
    if (!bInfiniteSpawns && SpawnsHappened >= MaxCountOfSpawns) { Destroy(); }
}

void AHSRandomSpawner::OnActorDestroyed(AActor* DestroyedActor)
{
    RandomSpawn();
}

FVector AHSRandomSpawner::GetRandomLocation() const
{
    const FVector Extent = BoxComponent->GetScaledBoxExtent();
    const double RandomX = FMath::RandRange(Extent.X, -Extent.X);
    const double RandomY = FMath::RandRange(Extent.Y, -Extent.Y);
    const double RandomZ = FMath::RandRange(Extent.Z, -Extent.Z);
    const FVector RandomRelativeLocation(RandomX, RandomY, RandomZ);
    return GetActorLocation() + RandomRelativeLocation;
}

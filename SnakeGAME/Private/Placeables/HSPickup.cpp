// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Placeables/HSPickup.h"

#include "Kismet/GameplayStatics.h"

AHSPickup::AHSPickup()
{
    PrimaryActorTick.bCanEverTick = false;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
    SetRootComponent(StaticMeshComponent);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetEnableGravity(true);
    SetRootComponent(StaticMeshComponent);
    bReplicates = true;
    StaticMeshComponent->SetIsReplicated(true);
}

void AHSPickup::BeginPlay()
{
    Super::BeginPlay();
}

void AHSPickup::PickedUp_Implementation()
{
    UGameplayStatics::PlaySoundAtLocation(this, GetOnEatingSound(), GetActorLocation(), GetActorRotation());
    Destroy();
}

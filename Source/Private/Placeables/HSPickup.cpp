// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Placeables/HSPickup.h"

#include "Components/CapsuleComponent.h"
#include "Components/HSHeadComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSPickup, All, All)

AHSPickup::AHSPickup()
{
    PrimaryActorTick.bCanEverTick = false;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
    SetRootComponent(StaticMeshComponent);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetEnableGravity(true);
    SetRootComponent(StaticMeshComponent);
}

void AHSPickup::BeginPlay()
{
    Super::BeginPlay();
    StaticMeshComponent->OnComponentHit.AddDynamic(this, &AHSPickup::OnHit);
    UE_LOG(LogHSPickup, Display, TEXT("Begin play! Name: %s Authority: %i"), *GetName(), HasAuthority())
}

void AHSPickup::PickedUp()
{
    UGameplayStatics::PlaySoundAtLocation(this, GetOnEatingSound(), GetActorLocation(), GetActorRotation());
    Destroy();
    UE_LOG(LogHSPickup, Display, TEXT("Picked up and destroyed"))
}

void AHSPickup::ApplyToActor(AActor* Actor)
{
    PickedUp();
}

void AHSPickup::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    const auto Character = Cast<ACharacter>(OtherActor);
    if (!Character) { return; }
    if (Cast<UCapsuleComponent>(OtherComp) != Character->GetCapsuleComponent()) { return; }
    UE_LOG(LogHSPickup, Display, TEXT("Hit character's primal capsule"))
    ApplyToActor(OtherActor);
}
